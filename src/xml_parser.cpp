//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/xml_parser.hpp>
#include <protean/object_proxy.hpp>
#include <protean/object_factory.hpp>
#include <protean/detail/scoped_xmlch.hpp>

#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace protean {

    stream_resolver::stream_resolver(const std::string& entity_path) :
        m_entity_path(boost::filesystem::system_complete(boost::filesystem::path(entity_path, boost::filesystem::native)))
    {
    }

    xercesc::InputSource* stream_resolver::resolveEntity(
        const XMLCh* const /*publicId*/,
        const XMLCh* const systemId )
    {
        xercesc::InputSource* result = NULL;

        std::string name = sax_content_handler::transcode(systemId);
        entity_stream_map_t::iterator it(m_entities.find(name));
        if (it!=m_entities.end())
        {
            std::istream* stream = it->second;
            result = new stream_input_source(*stream);
            m_entities.erase(it);
        }
        else
        {
            boost::filesystem::path full_path = boost::filesystem::complete(boost::filesystem::path(name, boost::filesystem::native), m_entity_path);
            if (boost::filesystem::exists(full_path))
            {
                scoped_xmlch full_path_str( xercesc::XMLString::transcode(full_path.native_directory_string().c_str()) );
                result = new xercesc::LocalFileInputSource(full_path_str.get());
            }
        }
        return result;
    }

    void stream_resolver::add_entity_stream(const std::string& name, std::istream* is)
    {
        entity_stream_map_t::const_iterator it(m_entities.find(name));
        if (it!=m_entities.end())
        {
            boost::throw_exception(variant_error(std::string("Duplicate entity stream ") + name + " passed to parser"));
        }
        m_entities.insert(make_pair(name, is));
    }

    sax_content_handler::sax_content_handler(variant& result, int flags, object_factory* factory) :
         m_locator(NULL),
         m_result(result),
         m_instructions(variant::Bag),
         m_flags(flags),
         m_factory(factory)
    {
    }

    void sax_content_handler::setDocumentLocator(const xercesc::Locator* const locator)
    {
        m_locator = locator;
    }

    void sax_content_handler::startElement(
            const XMLCh* const /*uri*/,
            const XMLCh* const /*localname*/,
            const XMLCh* const qname,
            const xercesc::Attributes& attrs )
    {
        try
        {
            const std::string element_name = transcode(qname);

            boost::shared_ptr<element_info> context(new element_info());

            get_attributes(attrs, context->attributes());

            boost::shared_ptr<element_info> parent_context;
            if (m_stack.empty())
            {
                if ((m_flags & xml_reader::Preserve)!=0)
                {
                    // create document node
                    m_result = variant(variant::Dictionary);
                    m_result.insert( "__instrs__", m_instructions );
                    context->m_element = &m_result.insert(element_name, variant(), variant::ReturnItem);
                }
                else
                {
                    context->m_element = &m_result;
                }
            }
            else
            {
                parent_context = m_stack.top();

                switch(parent_context->m_type)
                {
                case variant::Any:
                    if (!parent_context->m_typed)
                    {
                        // untyped elements with children are assumed
                        // to be bags
                        parent_context->m_type = variant::Bag;
                        parent_context->element() = variant(variant::Bag);
                        context->m_element = &parent_context->element().insert(element_name, variant(), variant::ReturnItem);
                    }
                    break;
                case variant::None:
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
                case variant::Buffer:
                    boost::throw_exception(variant_error("Unexpected start element"));
                case variant::List:
                    context->m_element = &parent_context->element().push_back(variant(), variant::ReturnItem);
                    break;
                case variant::Dictionary:
                case variant::Bag:
                    context->m_element = &parent_context->element().insert(element_name, variant(), variant::ReturnItem);
                    break;
                case variant::Tuple:
                    context->m_element = &parent_context->element()[parent_context->m_num_row++];
                    break;
                case variant::TimeSeries:
                    if ( context->attributes().has_key("time") )
                    {
                        variant::date_time_t time = context->attributes()["time"].as<variant::date_time_t>();
                        context->attributes().remove("time");
                        context->m_element = &parent_context->element().push_back(time, variant(), variant::ReturnItem);
                    }
                    else
                    {
                        boost::throw_exception(variant_error("Missing 'time' attribute from TimeSeries member"));
                    }
                    break;
                case variant::Exception:
                case variant::Object:
                    // Exception and Object are serialised using a Dictionary
                    context->m_element = &parent_context->element().insert(element_name, variant(), variant::ReturnItem);
                    break;
                case variant::Array:
                    // Arrays are serialised using a Tuple
                    context->m_element = &parent_context->element()[parent_context->m_num_row++];
                    break;
                default:
                    boost::throw_exception(variant_error(std::string("Unrecognised variant type: ") + variant::enum_to_string(parent_context->m_type)));
                }
            }

            m_stack.push(context);

            // infer type from 'variant' attribute
            if (context->attributes().has_key("variant"))
            {
                const std::string& type_str = context->attributes()["variant"].as<std::string>();
                context->m_type = variant::string_to_enum(type_str);
                context->m_typed = true;
                context->attributes().remove("variant");
            }

            // infer type from parent (Array only)
            if (parent_context && parent_context->m_type==variant::Array)
            {
                if (!context->m_typed)
                {
                    context->m_type = parent_context->m_item_type;
                    context->m_typed = true;
                }
                else if (context->m_type!=parent_context->m_item_type)
                {
                    boost::throw_exception(variant_error("Type of array does not match type of child element"));
                }
            }

            switch(context->m_type)
            {
                case variant::Any:
                    if (!context->m_typed && !context->attributes().empty())
                    {
                        // untyped elements with attributes are assumed
                        // to be bags
                        context->m_type = variant::Bag;
                        context->element() = variant(variant::Bag);
                    }
                    break;
                case variant::None:
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
                case variant::Buffer:
                    break;
                case variant::List:
                case variant::Dictionary:
                case variant::Bag:
                case variant::TimeSeries:
                    context->element() = variant(context->m_type);
                    break;
                case variant::Exception:
                case variant::Object:
                    // Exception and Object are parsed via a dictionary for convenience.
                    context->element() = variant(variant::Dictionary);
                    break;
                case variant::Tuple:
                {
                    if (context->attributes().has_key("size"))
                    {
                        context->m_num_rows = context->attributes()["size"].as<size_t>();
                        context->attributes().remove("size");
                    }
                    else
                    {
                        boost::throw_exception(variant_error("Missing 'size' attribute for Tuple variant"));
                    }
                    context->element() = variant(variant::Tuple, context->m_num_rows);
                    break;
                }
                case variant::Array:
                {
                    if (context->attributes().has_key("size"))
                    {
                        context->m_num_rows = context->attributes()["size"].as<size_t>();
                        context->attributes().remove("size");
                    }
                    else
                    {
                        boost::throw_exception(variant_error("Missing 'size' attribute for Array variant"));
                    }
                    if (context->attributes().has_key("type"))
                    {
                        const std::string& type_str = context->attributes()["type"].as<std::string>();
                        context->m_item_type = variant::string_to_enum(type_str);
                        context->attributes().remove("type");
                    }
                    else
                    {
                        boost::throw_exception(variant_error("Missing 'type' attribute for Array variant"));
                    }
                    context->element() = variant(variant::Tuple, context->m_num_rows);
                    break;
                }
                default:
                    boost::throw_exception(variant_error(std::string("Unrecognised variant type: ") + variant::enum_to_string(context->m_type)));
            }
        }
        catch( const std::exception &e )
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

    void sax_content_handler::endElement(
        const XMLCh* const /*uri*/,
        const XMLCh* const /*localname*/,
        const XMLCh* const /*qname*/ )
    {
        try
        {
            if ( m_stack.empty() )
            {
                boost::throw_exception(variant_error("End element encountered without corresponding start element"));
            }

            boost::shared_ptr<element_info> context(m_stack.top());

            if (!context->attributes().empty())
            {
                if (context->element().is<variant::Mapping>())
                {
                    if ((m_flags & xml_reader::Preserve)!=0)
                    {
                        context->element().insert("__attrs__", context->attributes());
                    }
                    else
                    {
                        variant::const_iterator it, end = context->attributes().end();
                        for(it=context->attributes().begin(); it!=end; ++it)
                        {
                            context->element().insert(it.key(), it.value());
                        }
                    }
                }
                else
                {
                    boost::throw_exception(variant_error(std::string("No attributes allowed in XML element of ") + variant::enum_to_string(context->m_type)));
                }
            }

            if ( (context->m_type & variant::Primitive)!=0 )
            {
                context->element() = variant(context->m_type, context->m_data);
            }
            else if ( context->m_type==variant::Exception )
            {
                std::string type_str, message_str, source_str, stack_str;

                type_str = context->element()["type"].as<std::string>();

                message_str = context->element()["message"].as<std::string>();

                if (context->element().has_key("source"))
                {
                    source_str = context->element()["source"].as<std::string>();
                }

                if (context->element().has_key("stack"))
                {
                    stack_str = context->element()["stack"].as<std::string>();
                }

                context->element() = exception_data(type_str, message_str, source_str, stack_str);
            }
            else if ( context->m_type==variant::Object )
            {
                std::string class_name(context->element()["class"].as<std::string>());
                int version(context->element()["version"].as<int>());

                handle<object> obj;
                if ( m_factory!=NULL )
                {
                    obj = m_factory->create_instance(class_name);
                    if (obj.null())
                    {
                        if ((m_flags & xml_reader::CreateProxy)!=0)
                        {
                            boost::throw_exception(variant_error("Unable to create object from factory"));
                        }
                    }
                }

                if (obj.null())
                {
                    obj = new object_proxy(class_name);
                }

                obj->inflate(context->element()["params"], version);

                context->element() = variant(obj);
            }
            else if ( context->m_type==variant::Buffer )
            {
                if ( !context->m_data.empty() )
                {
                    XMLSize_t n = 0;
                    boost::scoped_ptr<XMLByte> data(xercesc::Base64::decode(reinterpret_cast<const XMLByte*>(context->m_data.c_str()), &n));
                    if (!data.get())
                    {
                        boost::throw_exception(variant_error("Unable to base64 decode data"));
                    }
                    context->element() = variant(data.get(), n);
                }
                else
                {
                    context->element() = variant(NULL, 0);
                }
            }
            else if ( context->m_type==variant::Array )
            {
                typed_array a(context->m_num_rows, context->m_item_type);
                for (size_t i=0; i<context->m_num_rows; ++i)
                {
                    a.at(i).swap(context->element()[i]);
                }
                context->element() = a;
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

    void sax_content_handler::characters(
        const XMLCh *const chars,
        const XMLSize_t /*length*/ )
    {
        try
        {
            // get a pointer to the top of the stack (if any)
            if ( m_stack.empty() ) return;

            boost::shared_ptr<element_info> context(m_stack.top());

            switch ( context->m_type )
            {
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
            case variant::Buffer:
                context->m_data.append( boost::algorithm::trim_copy(transcode(chars)));
                break;
            case variant::None:
            case variant::List:
            case variant::Tuple:
            case variant::Dictionary:
            case variant::Bag:
            case variant::TimeSeries:
            case variant::Exception:
            case variant::Object:
            case variant::Array:
                if (!is_ws(chars))
                {
                    boost::throw_exception(variant_error("Unexpected characters encountered"));
                };
                break;
            default:
                boost::throw_exception(variant_error("Case exhaustion: " + variant::enum_to_string(context->m_type))); 
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

    void sax_content_handler::processingInstruction(
        const XMLCh *const target,
        const XMLCh *const data )
    {
        const std::string target_str = transcode(target);
        const std::string data_str = transcode(data);

        m_instructions.insert(target_str, variant(data_str));
    }

    void sax_content_handler::comment(
        const XMLCh* const chars, 
        const XMLSize_t /*length*/
    )
    {
        //will only enter when Preserve flag is specified.  Warning: dictionary will lexicographically sort contents (incl comments)
        if (!m_stack.empty())
        {
            boost::shared_ptr<element_info> context(m_stack.top());
            if(context->m_type == variant::Any)
            {
                context->m_type = variant::Bag;
                context->element() = variant(variant::Bag);
            }
            if (context->m_type == variant::Bag)
            {
                context->element().insert("__comment__", variant(transcode(chars)));
            }
            else
            {
                boost::throw_exception(variant_error("Comments must be contained in Bag variants"));
            }
        }
    }

    typedef std::map<std::string, variant::enum_type_t> psvi_enum_map_t;
    static const psvi_enum_map_t psvi_enum_map = boost::assign::map_list_of
        ("string",            variant::String   )
        ("integer",           variant::Int32    ) // arbitrary width integer, so this is wrong!
        ("positiveInteger",   variant::UInt32   ) // arbitrary width integer, so this is wrong!
        ("int",               variant::Int32    )
        ("unsignedInt",       variant::UInt32   )
        ("long",              variant::Int64    )
        ("unsignedLong",      variant::UInt64   )
        ("float",             variant::Float    )
        ("double",            variant::Double   )
        ("boolean",           variant::Boolean  )
        ("date",              variant::Date     )
        ("time",              variant::Time     )
        ("duration",          variant::Time     )
        ("dateTime",          variant::DateTime )
        ("base64",            variant::Buffer   );

    static variant::enum_type_t xsd_type_to_enum(const std::string& type)
    {
        psvi_enum_map_t::const_iterator it(psvi_enum_map.find(type));
        if (it!=psvi_enum_map.end())
        {
            return it->second;
        }
        return variant::None;
    }

    variant::enum_type_t sax_content_handler::derived_from_type(xercesc::XSTypeDefinition* type_def)
    {
        xercesc::XSTypeDefinition* current = type_def;

        while (current && current!=current->getBaseType())
        {
            if( !current->getAnonymous() )
            {
                const std::string name = transcode(current->getName());
                variant::enum_type_t type = xsd_type_to_enum(name);
                if (type!=variant::None)
                {
                    return type;
                }
            }
            current = current->getBaseType();
        }
        return variant::Any;
    }

    void sax_content_handler::handleAttributesPSVI(
        const XMLCh* const /*localName*/, 
        const XMLCh* const /*uri*/, 
        xercesc::PSVIAttributeList* psviAttributes )
    {
        try
        {
            if ( m_stack.empty() )
            {
                boost::throw_exception(variant_error("handleAttributesPSVI called without corresponding start element"));
            }

            boost::shared_ptr<element_info> context(m_stack.top());

            infer_attribute_types_from_schema(psviAttributes, context->attributes());
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

    void sax_content_handler::handleElementPSVI(
        const XMLCh* const /*localName*/,
        const XMLCh* const /*uri*/,
        xercesc::PSVIElement* elementInfo )
    {
        try
        {
            if ( m_stack.empty() )
            {
                boost::throw_exception(variant_error("handleElementPSVI called without corresponding start element"));
            }

            boost::shared_ptr<element_info> context(m_stack.top());

            if (!context->m_typed)
            {
                variant::enum_type_t schema_type = derived_from_type(elementInfo->getTypeDefinition());

                if (schema_type!=variant::Any)
                {
                    context->m_type = schema_type;
                    context->m_typed = true;
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

    void sax_content_handler::warning(const xercesc::SAXParseException& e)
    {
        XMLFileLoc line_num = e.getLineNumber();
        std::string message(transcode(e.getMessage()));

        std::cerr << "XML parse warning: (line " << line_num << ") ";
        std::cerr << message << std::endl;
    }

    void sax_content_handler::error(const xercesc::SAXParseException& e)
    {
        XMLFileLoc line_num = e.getLineNumber();
        std::string message(transcode(e.getMessage()));

        if (line_num == 0)
        {
            boost::throw_exception(variant_error(message));
        }
        else
        {
            boost::throw_exception(variant_error((boost::format("line %d: %s") % static_cast<int>(line_num) % message).str()));
        }
    }

    void sax_content_handler::fatalError(const xercesc::SAXParseException& e)
    {
        XMLFileLoc line_num = e.getLineNumber();
        const std::string message(transcode(e.getMessage()));

        if (line_num == 0)
        {
            boost::throw_exception(variant_error(message));
        }
        else
        {
            boost::throw_exception(variant_error((boost::format("line %d: %s") % static_cast<int>(line_num) % message).str()));
        }
    }

    /*static*/ bool sax_content_handler::is_ws(const XMLCh* const arg)
    {
        return xercesc::XMLChar1_0::isAllSpaces(arg, xercesc::XMLString::stringLen(arg));
    }

    /*static*/ std::string sax_content_handler::transcode(const XMLCh* const arg)
    {
        char* transcoded = xercesc::XMLString::transcode(arg);
        std::string tmp(transcoded);
        xercesc::XMLString::release(&transcoded);

        return tmp;
    }

    /*static*/ void sax_content_handler::get_attributes(const xercesc::Attributes& attrs, variant& result)
    {
        for ( unsigned int i=0; i<attrs.getLength(); ++i )
        {
            std::string name  = transcode(attrs.getQName(i));
            std::string value = transcode(attrs.getValue(i));

            // make sure we remove any schema related attributes
            if (name.find("xsi")==std::string::npos)
            {
                result.insert(name, variant(variant::Any, value));
            }
        }
    }

    /*static*/ void sax_content_handler::infer_attribute_types_from_schema(xercesc::PSVIAttributeList* schemaAttrs, variant& attrs)
    {
        for (size_t i=0; i<schemaAttrs->getLength(); ++i)
        {
            xercesc::PSVIAttribute *attr = schemaAttrs->getAttributePSVIAtIndex( i );

            std::string name = transcode(schemaAttrs->getAttributeNameAtIndex(i));

            // attempt to infer base type from schema
            variant::enum_type_t type = derived_from_type(attr->getTypeDefinition());
            if (attrs.has_key(name))
            {
                attrs[name] = variant(type, attrs[name].as<std::string>());
            }
        }
    }


} // namespace protean
