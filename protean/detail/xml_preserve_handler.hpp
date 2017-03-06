//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DETAIL_XML_PRESERVE_HANDLER_HPP
#define PROTEAN_DETAIL_XML_PRESERVE_HANDLER_HPP

#include <protean/config.hpp>

#include <protean/detail/xml_handler_base.hpp>
#include <protean/variant.hpp>

namespace protean { namespace detail {

    class xml_preserve_handler :
        public xml_handler_base
    {
    public:
        xml_preserve_handler(variant& result, int flags);

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

        class element_info
        {
        public:
            element_info() :
                m_element(nullptr)
            {}
        public:
            variant& element() { return *m_element; }
        private:
            variant*    m_element;

            friend class xml_preserve_handler;
        };

        std::stack<boost::shared_ptr<element_info> > m_stack;

    };

}} // namespace protean::detail

#endif // PROTEAN_DETAIL_XML_PRESERVE_HANDLER_HPP
