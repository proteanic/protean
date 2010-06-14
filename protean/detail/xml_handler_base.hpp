//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DETAIL_XML_HANDLER_BASE_HPP
#define PROTEAN_DETAIL_XML_HANDLER_BASE_HPP

#include <protean/config.hpp>

#include <protean/variant.hpp>
#include <protean/detail/xerces_include.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>

namespace protean { namespace detail {

    class xml_stream_resolver : public xercesc::EntityResolver
    {
    public:
        xml_stream_resolver(const std::string& entity_path);

        xercesc::InputSource* resolveEntity(
            const XMLCh* const publicId,
            const XMLCh* const systemId );

        void add_entity_stream(const std::string& id, std::istream* is);
    private:
        typedef std::map<std::string, std::istream*> entity_stream_map_t;
        entity_stream_map_t    m_entities;

        boost::filesystem::path m_entity_path;
    };

    class xml_stream_source : public xercesc::InputSource
    {
    private:
        class xml_bin_stream : public xercesc::BinInputStream
        {
        public:
            xml_bin_stream(std::istream& is);

            XMLFilePos curPos() const;
            XMLSize_t readBytes(XMLByte *const toFill, const XMLSize_t maxToRead);
            const XMLCh* getContentType() const;

        private:
            std::istream&   m_is;
            size_t          m_pos;
        };

    public:
        xml_stream_source(std::istream& is);
        ~xml_stream_source();

        xercesc::BinInputStream* makeStream() const;

    private:
        std::istream&    m_is;
    };

    class xml_handler_base :
        public xercesc::DefaultHandler,
        public xercesc::PSVIHandler
    {
    public:
        xml_handler_base(variant& result, int mode);
        virtual ~xml_handler_base();

        void setDocumentLocator(const xercesc::Locator* const locator);

        void warning( const xercesc::SAXParseException& e );
        void error( const xercesc::SAXParseException& e );
        void fatalError( const xercesc::SAXParseException& e );

    protected:
        const xercesc::Locator* m_locator;
        variant&                m_result;
        int                     m_mode;
    };

}} // namespace protean::detail

#endif // PROTEAN_DETAIL_XML_HANDLER_BASE_HPP
