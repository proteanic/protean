#include <protean/detail/xml_preserve_handler.hpp>
#include <protean/detail/xml_utility.hpp>
#include <protean/xml_common.hpp>

#include <boost/format.hpp>

namespace protean { namespace detail {

    xml_preserve_handler::xml_preserve_handler(variant& result, int mode) :
        xml_handler_base(result, mode)
    {
        // Create document node
        m_result = variant(variant::Bag);

        boost::shared_ptr<element_info> context(new element_info());
        context->m_element = &m_result;
        m_stack.push(context);
    }

    void xml_preserve_handler::startElement(
            const XMLCh* const /*uri*/,
            const XMLCh* const /*localname*/,
            const XMLCh* const qname,
            const xercesc::Attributes& attrs )
    {
        try
        {
            const std::string element_name = xml_utility::transcode(qname);

            boost::shared_ptr<element_info> context(new element_info());

            context->m_element = &m_stack.top()->element().insert(element_name, variant(variant::Bag), variant::ReturnItem);

            variant attributes(variant::Bag);
            xml_utility::get_attributes(attrs, attributes);

            if (attributes.size()>0)
            {
                context->m_element->insert(xml_attributes, attributes);
            }

            m_stack.push(context);
        }
        catch(const std::exception& e)
        {
            if (m_locator)
            {
                XMLFileLoc lineNo = m_locator->getLineNumber();
                boost::throw_exception(variant_error((boost::format("line %d: %s") % (int)lineNo % e.what()).str()));
            }
            else
            {
                boost::throw_exception(variant_error(e.what()));
            }
        }
        catch(...)
        {
            boost::throw_exception(variant_error("Unhandled exception"));
        }
    }

    void xml_preserve_handler::endElement(
        const XMLCh* const /*uri*/,
        const XMLCh* const /*localname*/,
        const XMLCh* const /*qname*/ )
    {
        try
        {
            if (m_stack.empty())
            {
                boost::throw_exception(variant_error("End element encountered without corresponding start element"));
            }
            m_stack.pop();
        }
        catch(const std::exception &e)
        {
            if (m_locator)
            {
                XMLFileLoc line_num = m_locator->getLineNumber();
                boost::throw_exception(variant_error((boost::format("line %d: %s") % static_cast<int>(line_num) % e.what()).str()));
            }
            else
            {
                boost::throw_exception(variant_error(e.what()));
            }
        }
        catch(...)
        {
            boost::throw_exception(variant_error("Unhandled exception"));
        }
    }

    void xml_preserve_handler::characters(
        const XMLCh *const chars,
        const XMLSize_t /*length*/ )
    {
        try
        {
            if (!m_stack.empty())
            {
                if ((m_mode & xml_mode::StripSpace)!=0 && xml_utility::is_ws(chars))
                {
                    // do nothing
                }
                else
                {
                    std::string text(xml_utility::transcode(chars));
                    m_stack.top()->m_element->insert(xml_text, variant(variant::Any, text));
                }
            }
        }
        catch(const std::exception &e)
        {
            if (m_locator)
            {
                XMLFileLoc line_num = m_locator->getLineNumber();
                boost::throw_exception(variant_error((boost::format("line %d: %s") % static_cast<int>(line_num) % e.what()).str()));
            }
            else
            {
                boost::throw_exception(variant_error(e.what()));
            }
        }
        catch(...)
        {
            boost::throw_exception(variant_error("Unhandled exception"));
        }
    }

    void xml_preserve_handler::processingInstruction(
        const XMLCh *const target,
        const XMLCh *const data )
    {
        if (!m_stack.empty())
        {
            boost::shared_ptr<element_info> context(m_stack.top());

            const std::string target_str = xml_utility::transcode(target);
            const std::string data_str = xml_utility::transcode(data);

            variant& instruction = context->element().insert(xml_instruction, variant(variant::Dictionary), variant::ReturnItem);
            instruction.insert(xml_target, variant(target_str));
            instruction.insert(xml_data, variant(data_str));
        }
    }

    void xml_preserve_handler::comment(
        const XMLCh* const chars, 
        const XMLSize_t /*length*/ )
    {
        if (!m_stack.empty())
        {
            boost::shared_ptr<element_info> context(m_stack.top());
            context->element().insert(xml_comment, variant(xml_utility::transcode(chars)));
        }
    }

    void xml_preserve_handler::handleAttributesPSVI(
        const XMLCh* const /*localName*/, 
        const XMLCh* const /*uri*/, 
        xercesc::PSVIAttributeList* /*psviAttributes*/ )
    { }
        

    void xml_preserve_handler::handleElementPSVI(
        const XMLCh* const /*localName*/,
        const XMLCh* const /*uri*/,
        xercesc::PSVIElement* /*elementInfo*/ )
    {  }

}} // namespace protean::detail
