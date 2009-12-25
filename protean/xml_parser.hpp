//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_XML_PARSER_HPP
#define PROTEAN_XML_PARSER_HPP

#include <protean/config.hpp>

#include <protean/xml_reader.hpp>
#include <protean/variant.hpp>
#include <protean/xml_defs.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>

namespace protean {

    class stream_input_source : public xercesc::InputSource
    {
    private:
        class stream_bin_input_source : public xercesc::BinInputStream
        {
        public:
            stream_bin_input_source(std::istream& is) :
                m_is(is),
                m_pos(0)
            {
            }
            virtual XMLFilePos curPos() const { return m_pos; }
            virtual XMLSize_t readBytes( XMLByte *const toFill, const XMLSize_t maxToRead )
            {
                try
                {
                    m_is.read(reinterpret_cast<char* const>(toFill), maxToRead);
                    m_pos += m_is.gcount();
                    return m_is.gcount();
                }
                catch(...)
                {
                    boost::throw_exception(variant_error("Exception occurred whilst reading from stream"));
                }
            }
            virtual const XMLCh* getContentType() const { return 0; }
        private:
            std::istream&   m_is;
            size_t          m_pos;
        };
    public:
        stream_input_source(std::istream& is) :
            m_is( is ) 
        {
            if(!is.good())
            {
                boost::throw_exception(variant_error("Input stream is bad"));
            }
        }
        ~stream_input_source()
        {
            m_is.clear();
        }
        virtual xercesc::BinInputStream* makeStream() const 
        {
            return new stream_bin_input_source( m_is );
        }

    private:
        std::istream&    m_is;
    };

    class stream_resolver : public xercesc::EntityResolver
    {
    public:
        stream_resolver(const std::string& entity_path);

        xercesc::InputSource* resolveEntity(
            const XMLCh* const publicId,
            const XMLCh* const systemId );

        void add_entity_stream(const std::string& id, std::istream* is);
    private:
        typedef std::map<std::string, std::istream*> entity_stream_map_t;
        entity_stream_map_t    m_entities;

        boost::filesystem::path m_entity_path;
    };

    class object_factory;

    class sax_content_handler :
        public xercesc::DefaultHandler,
        public xercesc::PSVIHandler
    {
    public:
        sax_content_handler(variant& result, int flags, object_factory* factory);

        void setDocumentLocator(const xercesc::Locator* const locator);

        // Over-ride Xerces document element callbacks
        void startElement(
            const XMLCh* const uri,
            const XMLCh* const localname,
            const XMLCh* const qname,
            const xercesc::Attributes& attributes );

        void endElement(
            const XMLCh* const uri,
            const XMLCh* const localname,
            const XMLCh* const qname );

        void characters(
            const XMLCh *const chars,
            const XMLSize_t length );

        void processingInstruction(
            const XMLCh *const target,
            const XMLCh *const data );
     
        // And the PSVI callbacks
        void handleAttributesPSVI(
            const XMLCh* const localName, 
            const XMLCh* const uri, 
            xercesc::PSVIAttributeList* psviAttributes );

        void handleElementPSVI(
            const XMLCh* const localName, 
            const XMLCh* const uri,
            xercesc::PSVIElement* elementInfo );

        //LexicalHandler callbacks
        void comment(
            const XMLCh* const chars, 
            const XMLSize_t length
        );

        void warning( const xercesc::SAXParseException& e );
        void error( const xercesc::SAXParseException& e );
        void fatalError( const xercesc::SAXParseException& e );

        // helpers
        static bool is_ws(const XMLCh* const arg);
        static std::string transcode(const XMLCh* const arg);
        static void get_attributes(const xercesc::Attributes& attrs, variant& result);
        static variant::enum_type_t derived_from_type(xercesc::XSTypeDefinition* type_def);
        static void infer_attribute_types_from_schema(xercesc::PSVIAttributeList* schemaAttrs, variant& attrs);

        class element_info
        {
        public:
            element_info() :
                m_type(variant::None),
                m_attributes(variant::Bag),
                m_element(NULL),
                num_rows(0),
                num_row(0)
            {}
        public:
            variant& attributes()        { return m_attributes; }
            variant& element()            { return *m_element; }
        private:
            std::string             m_data;
            variant::enum_type_t    m_type;
            variant                 m_attributes;
            variant*                m_element;

            // used for serialising tuples
            size_t                num_rows;
            size_t                num_row;

            friend class sax_content_handler;
        };

        const xercesc::Locator*     m_locator;
        variant&                    m_result;
        variant                     m_instructions; // processing instructions
        std::stack<boost::shared_ptr<element_info> > m_stack;

        int                         m_flags;

        object_factory*             m_factory;
    };

} // namespace protean

#endif // PROTEAN_XML_PARSER_HPP
