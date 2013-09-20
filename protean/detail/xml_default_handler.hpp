//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DETAIL_XML_DEFAULT_HANDLER_HPP
#define PROTEAN_DETAIL_XML_DEFAULT_HANDLER_HPP

#include <protean/config.hpp>

#include <protean/detail/xml_handler_base.hpp>
#include <protean/variant.hpp>
#include <protean/detail/data_table_column_serializers.hpp>

namespace protean {

    class object_factory;
    
    namespace detail {

    class xml_default_handler :
        public xml_handler_base
    {
    public:
        xml_default_handler(variant& result, int flags, object_factory* factory);

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
     
        // And the PSVI callbacks
        void handleAttributesPSVI(
            const XMLCh* const localName, 
            const XMLCh* const uri, 
            xercesc::PSVIAttributeList* psviAttributes );

        void handleElementPSVI(
            const XMLCh* const localName, 
            const XMLCh* const uri,
            xercesc::PSVIElement* elementInfo );

        // helpers
        static variant::enum_type_t derived_from_type(xercesc::XSTypeDefinition* type_def);
        static void infer_attribute_types_from_schema(xercesc::PSVIAttributeList* schemaAttrs, variant& attrs);

        class element_info
        {
        public:
            element_info() :
                m_type(variant::Any),
                m_attributes(variant::Bag), // a bag to preserve ordering
                m_element(NULL),
                m_typed(false),
                m_num_rows(0),
                m_num_row(0),
                m_item_type(variant::None),
                m_column(std::string()),
                m_column_reader(NULL),
                m_num_columns(0),
                m_num_column(0),
                m_in_columns(false)
            {}
        public:
            variant& attributes()        { return m_attributes; }
            variant& element()            { return *m_element; }
        private:
            std::string             m_data;
            variant::enum_type_t    m_type;
            variant                 m_attributes;
            variant*                m_element;
            bool                    m_typed;

            // used for serialising tuples and arrays
            size_t                  m_num_rows;
            size_t                  m_num_row;

            // used for data tables
            std::string             m_column;
            boost::shared_ptr<
                std::stringstream
            >                       m_column_data;
            boost::shared_ptr<
                data_table_column_reader
            >                       m_column_reader;
            bool                    m_in_columns;
            size_t                  m_num_columns;
            size_t                  m_num_column;

            // used for arrays
            variant::enum_type_t    m_item_type;

            friend class xml_default_handler;
        };

        variant                     m_instructions; // processing instructions
        std::stack<boost::shared_ptr<element_info> > m_stack;

        object_factory*             m_factory;
    };

}} // namespace protean::detail

#endif // PROTEAN_DETAIL_XML_DEFAULT_HANDLER_HPP
