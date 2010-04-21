//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/xml_writer.hpp>
#include <protean/xml_defs.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/erase.hpp>

namespace protean {

    xml_writer::xml_writer(std::ostream &os, int flags) :
        m_indentation(0),
        m_os(os),
        m_flags(flags)
    {
        if(!os.good())
        {
            boost::throw_exception(variant_error("Output stream is bad"));
        }
    }

    void xml_writer::write_document(const variant &document)
    {
        try
        {
            if ((m_flags & NoHeader)==0)
            {
                // output the XML header
                write_header();
            }

            if ((m_flags & Preserve)!=0)
            {
                if ( document.is<variant::Dictionary>() )
                {
                    if ( document.has_key( "__instrs__" ) )
                    {
                        write_instructions( document["__instrs__"] );
                    }
                    size_t num_nodes(0);
                    variant::const_iterator it( document.begin() );
                    for ( ; it!=document.end(); ++it )
                    {
                        if ( it.key()!="__instrs__" )
                        {
                            if (++num_nodes>1)
                            {
                                boost::throw_exception(variant_error("Invalid document structure, root node contains multiple elements"));
                            }
                            push(it.key());
                            write_node(*it);
                            pop();
                        }
                    }
                }
                else
                {
                    boost::throw_exception(variant_error("Invalid document structure, root node must be a singleton Dictionary"));
                }
            }
            else
            {
                push();
                write_node(document);
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

    variant& xml_writer::push(const std::string& element_name)
    {
        m_stack.push(element_info(element_name));
        return m_stack.top().m_attributes;
    }
    void xml_writer::pop()
    {
        m_stack.pop();
    }

    void xml_writer::start_content()
    {
        m_os << "\n";
        ++m_indentation;
    }
    void xml_writer::end_content()
    {
        --m_indentation;
        indent();
    }

    void xml_writer::write_value(const variant& node)
    {
        try
        {
            element_info& context = m_stack.top();

            switch(node.type())
            {
            case variant::None:
            {
                start_tag();
                end_tag();
                break;
            }
            case variant::Any:
            case variant::String:
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
                start_tag();
                m_os << escape_chars(node.change_type(variant::Any).as<std::string>());
                end_tag();
                break;
            }
            case variant::Dictionary:
            case variant::Bag:
            {
                start_tag();
                start_content();

                variant::const_iterator it, end = node.end();
                for ( it=node.begin(); it!=end; ++it )
                {
                    if ((m_flags & Preserve)!=0 && it.key()=="__attrs__" )
                    {
                        continue;
                    }

                    push(it.key());
                    write_node(it.value());
                    pop();
                }

                end_content();
                end_tag();

                break;
            }
            case variant::List:
            {
                start_tag();
                start_content();

                BOOST_FOREACH(const variant& item, node)
                {
                    push();
                    write_node(item);
                    pop();
                }

                end_content();
                end_tag();

                break;
            }
            case variant::Tuple:
            {
                context.m_attributes.insert("size", variant(node.size()));
                start_tag();

                start_content();

                BOOST_FOREACH(const variant& item, node)
                {
                    push();
                    write_node(item);
                    pop();
                }

                end_content();

                end_tag();
                break;
            }
            case variant::TimeSeries:
            {
                start_tag();

                start_content();
                variant::const_iterator it, end = node.end();
                for (it=node.begin(); it!=end; ++it)
                {
                    push().insert("time", variant(it.time()));
                    write_node(it.value());
                    pop();
                }
                end_content();

                end_tag();
                break;
            }
            case variant::Buffer:
            {
                start_tag();
                void *data = node.as<void*>();
                size_t size = node.size();
                if (data!=NULL && size>0)
                {
                    XMLSize_t n = 0;
                    boost::scoped_ptr<XMLByte> b64( xercesc::Base64::encode( reinterpret_cast<const XMLByte*>( data ), (unsigned int)size, &n ) );
                    if (!b64.get())
                    {
                        boost::throw_exception(variant_error("Unable to base64 encode data"));
                    }
                    m_os << b64.get();
                }
                end_tag();
                break;
            }
            case variant::Object:
            {
                const object& obj(node.as<object>());

                // write class name
                m_stack.top().m_attributes
                    .insert("class", variant(obj.name()))
                    .insert("version", variant(obj.version()));

                start_tag();
                start_content();

                // write parameter dictionary
                variant params;
                obj.deflate(params);
                push("params");
                write_node( params );
                pop();

                end_content();
                end_tag();

                break;
            }
            case variant::Exception:
            {
                start_tag();
                start_content();

                exception_data e(node.as<exception_data>());

                push("type");
                write_node( variant(e.type()) );
                pop();
                
                push("message");
                write_node( variant(e.message()) );
                pop();

                if (!e.source().empty())
                {
                    write_node( variant(e.source()) );
                }

                if (!e.stack().empty())
                {
                    write_node( variant(e.stack()) );
                }

                end_content();
                end_tag();
                break;
            }
            case variant::Array:
            {
                const typed_array& a(node.as<typed_array>());
                context.m_attributes.insert("size", variant(a.size()));
                context.m_attributes.insert("type", variant(variant::enum_to_string(a.type())));
                start_tag();

                start_content();

                for (size_t i=0; i<a.size(); ++i)
                {
                    push();
                    write_value(variant(a[i]));
                    pop();
                }

                end_content();

                end_tag();
                break;
            }
	        default:
 	            boost::throw_exception(variant_error("Case exhaustion: " + variant::enum_to_string(node.type()))); 
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

    void xml_writer::write_node(const variant& node)
    {
        try
        {
            element_info& context = m_stack.top();
            variant::enum_type_t type = node.type();    
            check_invalid_chars(context.m_name);

            if(context.m_name=="__comment__")
            {
                if((m_flags & Preserve)!=0)
                {
                    if( type==variant::String )
                    {
                        indent();
                        m_os << "<!--" << node.as<std::string>() << "-->\n";
                        return;
                    }
                    else
                    {
                        boost::throw_exception(variant_error("Comment type must be a variant String"));
                    }        
                }
                else
                    return;
            }

            if ((m_flags & Preserve)!=0)
            {
                if ((type & variant::Mapping)!=0)
                {
                    if (node.has_key("__attrs__"))
                    {
                        context.m_attributes = node["__attrs__"];
                    }
                }
            }
            else
            {
                context.m_attributes.insert("variant", variant(variant::enum_to_string(type)));
            }

            write_value(node);
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
    void xml_writer::write_header()
    {
        m_os << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    }
    void xml_writer::write_instructions( const variant &instructions )
    {
        if (!instructions.is<variant::Mapping>())
        {
            boost::throw_exception(variant_error("Expecting associative variant for processing instructions"));
        }
        variant::const_iterator it( instructions.begin() );
        for( ; it!=instructions.end(); ++it )
        {
            m_os << "<?" << it.key() << " " << it.value().as<std::string>() << "?>\n";
        }
    }
    void xml_writer::start_tag()
    {
        element_info& context(m_stack.top());

        indent();
        m_os << '<' << ( context.m_name.empty() ? xml_default_element : context.m_name );

        variant::const_iterator it, end = context.m_attributes.end();
        for ( it=context.m_attributes.begin(); it!=end; ++it )
        {
            check_invalid_chars(it.key());
            m_os << ' ' << it.key() << "=\"" << it.value().change_type(variant::String).as<std::string>() << '"';
        }
        m_os << '>';
    }
    void xml_writer::end_tag()
    {
        element_info& context(m_stack.top());

        m_os << "</" << ( context.m_name.empty() ? xml_default_element : context.m_name ) << ">";

        if (m_stack.size()>1)
        {
            m_os << "\n";
        }
    }
    void xml_writer::indent()
    {
        for (size_t i=0; i<m_indentation; ++i)
        {
            m_os << "  ";
        }
    }

    /*static*/ std::string xml_writer::escape_chars(const std::string& arg)
    {
        std::string tmp(arg);

        // make sure '&' is escaped first, as it is introduced by the others
        boost::algorithm::replace_all(tmp, "&",  "&amp;");
        boost::algorithm::replace_all(tmp, "<",  "&lt;");
        return tmp;
    }

    /*static*/ void xml_writer::check_invalid_chars(const std::string& arg)
    {
        if(arg.find_first_of("&<>'\" ") != std::string::npos)
        {
            boost::throw_exception(variant_error("Invalid character in string '" + arg + "'"));
        }
    }

    xml_writer& operator<<(xml_writer& writer, const variant& v)
    {
        writer.write_document(v);
        return writer;
    }

} // namespace protean


