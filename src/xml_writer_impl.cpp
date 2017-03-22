//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/detail/xml_writer_impl.hpp>
#include <protean/detail/xml_utility.hpp>
#include <protean/detail/base64.hpp>
#include <protean/detail/data_table.hpp>
#include <protean/detail/data_table_column_serializers.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/foreach.hpp>

namespace protean { namespace detail {

    xml_writer_impl::xml_writer_impl(std::ostream& os, int mode) :
        m_os(os),
        m_mode(mode)
    {
        if(!os.good())
        {
            boost::throw_exception(variant_error("Output stream is bad"));
        }
    }

    variant& xml_writer_impl::push(const std::string& element_name)
    {
        int level(0);
        if (!m_stack.empty())
        {
            level = m_stack.top().m_level+1;
        }
        m_stack.push(element_info(element_name, level));
        return m_stack.top().m_attributes;
    }

    void xml_writer_impl::pop()
    {
        m_stack.pop();
    }

    void xml_writer_impl::write_header()
    {
        m_os << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    }

    std::string xml_writer_impl::indent() const
    {
        std::ostringstream oss;

        oss << "\n";
        if (!m_stack.empty())
        {
            if ((m_mode & xml_mode::Indent)!=0)
            {
                for (size_t i=0; i<m_stack.top().m_level; ++i)
                {
                   oss << "  ";
                }
            }
        }

        return oss.str();
    }

    void xml_writer_impl::write_empty_element()
    {
        if (!m_stack.empty())
        {
            const element_info& context(m_stack.top());
            
            detail::xml_utility::check_invalid_chars(context.m_name);

            m_os << '<' << (context.m_name.empty() ? xml_default_element : context.m_name);

            variant::const_iterator it, end = context.m_attributes.end();
            for ( it=context.m_attributes.begin(); it!=end; ++it )
            {
                detail::xml_utility::check_invalid_chars(it.key());
                m_os << ' ' << it.key() << "=\"" << detail::xml_utility::escape_chars(it.value().any_cast().as<std::string>()) << '"';
            }
            m_os << "/>";
        }
    }
    
    std::string xml_writer_impl::start_tag() const
    {
        std::ostringstream oss;

        if (!m_stack.empty())
        {
            const element_info& context(m_stack.top());
            
            detail::xml_utility::check_invalid_chars(context.m_name);

            oss << '<' << (context.m_name.empty() ? xml_default_element : context.m_name);

            variant::const_iterator it, end = context.m_attributes.end();
            for ( it=context.m_attributes.begin(); it!=end; ++it )
            {
                detail::xml_utility::check_invalid_chars(it.key());
                oss << ' ' << it.key() << "=\"" << detail::xml_utility::escape_chars(it.value().any_cast().as<std::string>()) << '"';
            }
            oss << '>';
        }

        return oss.str();
    }

    std::string xml_writer_impl::end_tag() const
    {
        std::ostringstream oss;

        if (!m_stack.empty())
        {
            const element_info& context(m_stack.top());

            oss << "</" << ( context.m_name.empty() ? xml_default_element : context.m_name ) << ">";
        }

        return oss.str();
    }

    void xml_writer_impl::write_document(const variant& document)
    {
        try
        {
            if ((m_mode & xml_mode::NoHeader)==0)
            {
                // output the XML header
                write_header();
            }

            if ((m_mode & xml_mode::Preserve)!=0)
            {
                bool first(true);
                if (document.is<variant::Mapping>())
                {
                    std::string element_name;
                    variant::const_iterator it, end(document.end());
                    for (it=document.begin(); it!=end; ++it)
                    {
                        if (it.key()==xml_text)
                        {
                            boost::throw_exception(variant_error("Encountered text in document node"));
                        }
                        else if (it.key()==xml_cdata)
                        {
                            boost::throw_exception(variant_error("Encountered CDATA in document node"));
                        }
                        else if (it.key()==xml_attributes)
                        {
                            boost::throw_exception(variant_error("Encountered attributes in document node"));
                        }
                        else if (it.key()==xml_instruction)
                        {
                            if (!first)
                            {
                                m_os << indent();
                            }
                            write_instruction(it.value());
                        }
                        else if (it.key()==xml_comment)
                        {
                            if (!first)
                            {
                                m_os << indent();
                            }
                            write_comment(it.value());
                        }
                        else
                        {
                            if (element_name.empty())
                            {
                                element_name = it.key();
                            }
                            else
                            {
                                boost::throw_exception(variant_error((boost::format("Illegal element %s encountered in document, expecting single element %s at root") % it.key() % element_name).str()));
                            }

                            push(it.key());
                            if (!first)
                            {
                                m_os << indent();
                            }
                            write_element(it.value());
                            pop();
                        }
                        first = false;
                    }
                }
                else
                {
                    boost::throw_exception(variant_error("Invalid document structure, root node must be a Dictionary or Bag"));
                }
            }
            else
            {
                push();
                write_variant(document);
                pop();
            }
        }
        catch (const std::exception &e)
        {
            boost::throw_exception(variant_error(e.what()));
        }
        catch (...)
        {
            boost::throw_exception(variant_error("Unhandled Exception"));
        }
    }

    void xml_writer_impl::write_instruction(const variant& instruction)
    {
        if (instruction.is<variant::Mapping>() && instruction.has_key(xml_target) && instruction.has_key(xml_data))
        {
            m_os << "<?" << instruction[xml_target].as<std::string>() << " " << instruction[xml_data].as<std::string>() << "?>";
        }
        else
        {
            boost::throw_exception(variant_error((boost::format("Expecting dictionary containing '%s' and '%s' for processing instruction") % xml_target % xml_data).str()));
        }        
    }

    void xml_writer_impl::write_comment(const variant& comment)
    {
        m_os << "<!--" << comment.as<std::string>() << "-->";
    }

    void xml_writer_impl::write_cdata(const variant& cdata)
    {
        m_os << "<![CDATA[" << cdata.as<std::string>() << "]]>";
    }

    void xml_writer_impl::write_text(const variant& text)
    {
        try
        {
            switch(text.type())
            {
            case variant::Any:
            case variant::String:
            {
                m_os << detail::xml_utility::escape_chars(text.as<std::string>());
                break;
            }
            case variant::Int32:
            case variant::UInt32:
            case variant::Int64:
            case variant::UInt64:
            case variant::Float:
            case variant::Double:
            case variant::Boolean:
            case variant::Date:
            case variant::Time:
            case variant::DateTime:
            {
                m_os << text.any_cast().as<std::string>();
                break;
            }
            default:
                boost::throw_exception(variant_error("Attempt to output non-primitive variant as text: " + variant::enum_to_string(text.type()))); 
            }
        }
        catch (const std::exception &e)
        {
            boost::throw_exception(variant_error(e.what()));
        }
        catch (...)
        {
            boost::throw_exception(variant_error("Unhandled Exception"));
        }
    }

    void xml_writer_impl::write_variant(const variant& value)
    {
        if ((m_mode & xml_mode::Preserve)==0)
        {
            m_stack.top().m_attributes.insert("variant", variant(variant::enum_to_string(value.type())));
        }

        write_element(value);
    }

    void xml_writer_impl::write_element(const variant& element)
    {
        try
        {
            if (element.is<variant::Collection>() && element.empty() && element.type() != variant::DataTable)
            {
                write_empty_element();
                return;
            }

            switch(element.type())
            {
            case variant::None:
            {
                write_empty_element();
                break;
            }
            case variant::Any:
            case variant::String:
            {
                if (element.as<std::string>().empty())
                {
                    write_empty_element();
                    break;
                }
            }
            case variant::Float:
            case variant::Double:
            case variant::Int32:
            case variant::UInt32:
            case variant::Int64:
            case variant::UInt64:
            case variant::Boolean:
            case variant::Date:
            case variant::Time:
            case variant::DateTime:
            {
                m_os << start_tag();
                write_text(element);
                m_os << end_tag();
                break;
            }
            case variant::Dictionary:
            case variant::Bag:
            {
                if ((m_mode & xml_mode::Preserve)!=0)
                {
                    if (element.has_key(xml_attributes))
                    {
                        m_stack.top().m_attributes = element[xml_attributes];

                        if (element.size()==1)
                        {
                            write_empty_element();
                            break;
                        }
                    }

                    m_os << start_tag();

                    bool prev_is_text = false;
                    variant::const_iterator it, end(element.end());
                    for (it=element.begin(); it!=end; ++it)
                    {
                        if (it.key()==xml_attributes)
                        {
                            continue;
                        }
                        else if (it.key()==xml_text)
                        {
                            write_text(it.value());
                            prev_is_text = true;
                        }
                        else if (it.key()==xml_cdata)
                        {
                            write_cdata(it.value());
                            prev_is_text = true;
                        }
                        else if (it.key()==xml_instruction)
                        {
                            push(it.key());

                            if (!prev_is_text)
                            {
                                m_os << indent();
                            }

                            write_instruction(it.value());

                            prev_is_text = false;

                            pop();
                        }
                        else if (it.key()==xml_comment)
                        {
                            push(it.key());

                            if (!prev_is_text)
                            {
                                m_os << indent();
                            }

                            write_comment(it.value());

                            prev_is_text = false;

                            pop();
                        }
                        else
                        {
                            push(it.key());

                            if (!prev_is_text)
                            {
                                m_os << indent();
                            }

                            write_element(it.value());

                            prev_is_text = false;

                            pop();
                        }
                    }

                    if (!prev_is_text)
                    {
                        m_os << indent();
                    }

                    m_os << end_tag();
                }
                else
                {
                    m_os << start_tag();

                    variant::const_iterator it, end(element.end());
                    for (it=element.begin(); it!=end; ++it)
                    {
                        push(it.key());

                        m_os << indent();
                        write_variant(it.value());

                        pop();
                    }

                    m_os << indent() << end_tag();
                }

                break;
            }
            case variant::List:
            {
                m_os << start_tag();

                BOOST_FOREACH(const variant& item, element)
                {
                    push();
                    m_os << indent();
                    write_variant(item);
                    pop();
                }

                m_os << indent();
                m_os << end_tag();

                break;
            }
            case variant::Tuple:
            {
                m_stack.top().m_attributes.insert("size", variant(element.size()));
                m_os << start_tag();

                BOOST_FOREACH(const variant& item, element)
                {
                    push();
                    m_os << indent();
                    write_variant(item);
                    pop();
                }

                m_os << indent();
                m_os << end_tag();
                break;
            }
            case variant::TimeSeries:
            {
                m_os << start_tag();

                variant::const_iterator it, end = element.end();
                for (it=element.begin(); it!=end; ++it)
                {
                    push().insert("time", variant(it.time()));
                    m_os << indent();
                    write_variant(it.value());
                    pop();
                }

                m_os << indent();
                m_os << end_tag();
                break;
            }
            case variant::DataTable:
            {
                const data_table& dt = element.m_value.get<variant::DataTable>();

                m_stack.top().m_attributes
                    .insert("rows",    variant(dt.size()))
                    .insert("columns", variant(dt.columns().size()));
                m_os << start_tag();

                if (!dt.columns().empty())
                {
                    push("Columns");
                    m_os << indent() << start_tag();

                    for (data_table::column_container_type::const_iterator column_iter = dt.columns().begin()
                           ; column_iter != dt.columns().end()
                           ; ++column_iter)
                    {
                        push("Column")
                            .insert("name", variant(column_iter->name()))
                            .insert("type", variant(variant::enum_to_string(column_iter->type())));
                        m_os << indent();
                        write_empty_element();
                        pop();
                    }

                    m_os << indent() << end_tag();
                    pop();
                }

                for (data_table::column_container_type::const_iterator column_iter = dt.columns().begin()
                       ; column_iter != dt.columns().end()
                       ; ++column_iter)
                {
                    push("Column").insert("name", variant(column_iter->name()));
                    m_os << indent() << start_tag();

                    if (column_iter->type() & variant_base::Primitive)
                    {
                        boost::scoped_ptr<data_table_column_writer> column_writer(
                            make_data_table_column_stream_writer(*column_iter, m_os)
                        );

                        while (column_writer->has_next())
                        {
                            push("V");
                            m_os << indent() << start_tag();
                            column_writer->write();
                            m_os << end_tag();
                            pop();

                            column_writer->advance();
                        }
                    }
                    else
                    {
                        variant::const_iterator iter(column_iter->begin());
                        variant::const_iterator end(column_iter->end());
                        while (iter != end)
                        {
                            push("V");
                            m_os << indent();
                            write_variant(*(iter++));
                            pop();
                        }
                    }

                    m_os << indent() << end_tag();
                    pop();
                }

                m_os << indent();
                m_os << end_tag();

                break;
            }
            case variant::Buffer:
            {
                m_os << start_tag();
                const void *data = element.as<void*>();
                size_t size = element.size();
                if (data!=NULL && size>0)
                {
                    unsigned int n = 0;
                    boost::scoped_ptr<char> b64(detail::b64_encode((const char*)data, (unsigned int)size, &n));
                    if (!b64.get())
                    {
                        boost::throw_exception(variant_error("Unable to base64 encode data"));
                    }
                    m_os << b64.get();
                }
                m_os << end_tag();
                break;
            }
            case variant::Object:
            {
                const object& obj(element.as<object>());

                // write class name
                m_stack.top().m_attributes
                    .insert("class", variant(obj.name()))
                    .insert("version", variant(obj.version()));

                m_os << start_tag();

                // write parameter dictionary
                variant params;
                obj.deflate(params);
                push("params");
                m_os << indent();
                write_variant(params);
                pop();

                m_os << indent();
                m_os << end_tag();

                break;
            }
            case variant::Exception:
            {
                m_os << start_tag();

                exception_data e(element.as<exception_data>());

                push("type");
                m_os << indent();
                write_element(variant(e.type()));
                pop();
                
                push("message");
                m_os << indent();
                write_element(variant(e.message()));
                pop();

                if (!e.source().empty())
                {
                    push("source");
                    m_os << indent();
                    write_element(variant(e.source()));
                    pop();
                }

                if (!e.stack().empty())
                {
                    push("stack");
                    m_os << indent();
                    write_element(variant(e.stack()));
                    pop();
                }

                m_os << indent();
                m_os << end_tag();
                break;
            }
            case variant::Array:
            {
                const typed_array& a(element.as<typed_array>());
                m_stack.top().m_attributes
                    .insert("size", variant(a.size()))
                    .insert("type", variant(variant::enum_to_string(a.type())));
                
                m_os << start_tag();

                for (size_t i=0; i<a.size(); ++i)
                {
                    push();
                    m_os << indent();
                    write_element(variant(a[i]));
                    pop();
                }

                m_os << indent();
                m_os << end_tag();
                break;
            }
            default:
                 boost::throw_exception(variant_error("Case exhaustion: " + variant::enum_to_string(element.type()))); 
            }
        }
        catch (const std::exception &e)
        {
            boost::throw_exception(variant_error(e.what()));
        }
        catch (...)
        {
            boost::throw_exception(variant_error("Unhandled Exception"));
        }
    }

}} // namespace protean::detail
