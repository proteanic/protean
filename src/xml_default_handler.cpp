//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/detail/xml_default_handler.hpp>
#include <protean/detail/xml_utility.hpp>
#include <protean/xml_common.hpp>
#include <protean/object_proxy.hpp>
#include <protean/object_factory.hpp>
#include <protean/detail/scoped_xmlch.hpp>
#include <protean/detail/data_table.hpp>

#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace protean { namespace detail {

    xml_default_handler::xml_default_handler(variant& result, int mode, object_factory* factory) :
        xml_handler_base(result, mode),
        m_instructions(variant::Bag),
        m_factory(factory)
    { }

    void xml_default_handler::startElement(
            const XMLCh* const /*uri*/,
            const XMLCh* const /*localname*/,
            const XMLCh* const qname,
            const xercesc::Attributes& attrs )
    {
        try
        {
            const std::string element_name = xml_utility::transcode(qname);

            boost::shared_ptr<element_info> context(new element_info());

            xml_utility::get_attributes(attrs, context->attributes());

            boost::shared_ptr<element_info> parent_context;
            if (m_stack.empty())
            {
                context->m_element = &m_result;
            }
            else
            {
                parent_context = m_stack.top();

                switch(parent_context->m_type)
                {
                case variant::Any:
                    if (!parent_context->m_typed)
                    {
                        // Untyped elements with children are converted to Bags
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
                case variant::DataTable:
                    context->m_type = variant::DataTable;
                    context->m_typed = true;
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
                case variant::DataTable:
                {
                    if (element_name == "Variant")
                    {
                        if (!context->attributes().has_key("rows"))
                            boost::throw_exception(variant_error("Missing 'rows' attribute for DataTable variant"));
                        if (!context->attributes().has_key("columns"))
                            boost::throw_exception(variant_error("Missing 'columns' attribute for DataTable variant"));

                        context->m_num_rows = context->attributes()["rows"].as<size_t>();
                        context->m_num_columns = context->attributes()["columns"].as<size_t>();
                        context->element() = variant(variant::DataTable, context->m_num_rows);

                        context->attributes().remove("rows");
                        context->attributes().remove("columns");
                    }

                    else if (element_name == "Column" && parent_context->m_in_columns)
                    {
                        if (!context->attributes().has_key("name"))
                            boost::throw_exception(variant_error("Missing 'name' attribute for DataTable column declaration"));
                        if (!context->attributes().has_key("type"))
                            boost::throw_exception(variant_error("Missing 'type' attribute for DataTable column declaration"));

                        context->m_element = &parent_context->element();
                        context->element().add_column(
                            variant::string_to_enum(context->attributes()["type"].as<std::string>()),
                            context->attributes()["name"].as<std::string>()
                        );

                        context->m_in_columns = true;
                        ++parent_context->m_num_column;

                        context->attributes().remove("name");
                        context->attributes().remove("type");
                    }

                    else if (element_name == "Column")
                    {
                        if (parent_context->m_num_column++ >= parent_context->m_num_columns)
                            boost::throw_exception(variant_error("Too many column definitions for DataTable"));
                        if (!context->attributes().has_key("name"))
                            boost::throw_exception(variant_error("Missing 'name' attribute for DataTable column"));

                        context->m_element = &parent_context->element();
                        data_table& dt = context->element().m_value.get<variant::DataTable>();

                        std::string column_name = context->attributes()["name"].as<std::string>();
                        try
                        {
                            context->m_num_rows = parent_context->m_num_rows;
                            context->m_num_columns = parent_context->m_num_columns;
                            context->m_column = context->attributes()["name"].as<std::string>();
                            context->attributes().remove("name");

                            data_table_column_base& column = dt.get_column(column_name);

                            // Default-allocate `rows' column values to be read into (no reallocation required since
                            // capacity of `rows' was specified in DataTable construction)
                            column.resize(context->m_num_rows);

                            context->m_column_data.reset(new std::stringstream());
                            context->m_column_reader.reset(
                                make_data_table_column_stream_reader(column, context->m_column_data)
                            );
                        }
                        catch (const variant_error& /*e*/)
                        {
                            boost::throw_exception(variant_error(boost::str(
                                boost::format("No such column '%s' was declared for DataTable variant") % column_name
                            )));
                        }
                    }

                    else if (element_name == "Columns")
                    {
                        context->m_element = &parent_context->element();
                        context->m_in_columns = true;
                        context->m_num_columns = parent_context->m_num_columns;
                    }

                    else
                    {
                        if (parent_context->m_column.empty())
                            boost::throw_exception(variant_error("Unrecognised DataTable entry encountered"));

                        context->m_element = &parent_context->element();
                        context->m_column_data = parent_context->m_column_data;
                        context->m_column_reader = parent_context->m_column_reader;
                        context->m_column = parent_context->m_column;
                        context->m_num_row = parent_context->m_num_row++;
                        context->m_num_rows = parent_context->m_num_rows;
                        context->m_num_columns = parent_context->m_num_columns;
                    }

                    break;
                }
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

    void xml_default_handler::endElement(
        const XMLCh* const /*uri*/,
        const XMLCh* const /*localname*/,
        const XMLCh* const qname )
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
                    variant::const_iterator it, end = context->attributes().end();
                    for(it=context->attributes().begin(); it!=end; ++it)
                    {
                        context->element().insert(it.key(), it.value());
                    }
                }
                else
                {
                    boost::throw_exception(variant_error(std::string("No attributes allowed in XML element of ") + variant::enum_to_string(context->m_type)));
                }
            }

            if ((context->m_type & variant::Primitive)!=0)
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
                        if ((m_mode & xml_mode::CreateProxy)!=0)
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
            else if ( context->m_type==variant::DataTable )
            {
                const std::string element_name = xml_utility::transcode(qname);

                if (element_name == "V")
                {
                    if (context->m_num_row >= context->m_num_rows)
                        boost::throw_exception(variant_error("Too many rows in DataTable column definition"));

                    context->m_column_data->clear();
                    *context->m_column_data << context->m_data;

                    context->m_column_reader->read();
                    context->m_column_reader->advance();
                }
                else if (element_name == "Columns")
                {
                    if (context->m_num_column != context->m_num_columns)
                        boost::throw_exception(variant_error("Unexpected number of column declarations for DataTable"));
                }
                else if (element_name == "Column" && !context->m_in_columns)
                {
                    if (context->m_num_row != context->m_num_rows)
                        boost::throw_exception(variant_error("Insufficient number of rows provided for DataTable"));
                }
                else if (element_name == "Variant")
                {
                    if (context->m_num_column != context->m_num_columns)
                        boost::throw_exception(variant_error("Insufficient number of column definitions for DataTable"));
                }
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

    void xml_default_handler::characters(
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
            case variant::DataTable:
                context->m_data.append(xml_utility::transcode(chars));
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
                if (!xml_utility::is_ws(chars))
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

    variant::enum_type_t xml_default_handler::derived_from_type(xercesc::XSTypeDefinition* type_def)
    {
        xercesc::XSTypeDefinition* current = type_def;

        while (current && current!=current->getBaseType())
        {
            if( !current->getAnonymous() )
            {
                const std::string name = xml_utility::transcode(current->getName());
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

    void xml_default_handler::handleAttributesPSVI(
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

    void xml_default_handler::handleElementPSVI(
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


    /*static*/ void xml_default_handler::infer_attribute_types_from_schema(xercesc::PSVIAttributeList* schemaAttrs, variant& attrs)
    {
        for (size_t i=0; i<schemaAttrs->getLength(); ++i)
        {
            xercesc::PSVIAttribute *attr = schemaAttrs->getAttributePSVIAtIndex( i );

            std::string name = xml_utility::transcode(schemaAttrs->getAttributeNameAtIndex(i));

            // attempt to infer base type from schema
            variant::enum_type_t type = derived_from_type(attr->getTypeDefinition());
            if (attrs.has_key(name))
            {
                attrs[name] = variant(type, attrs[name].as<std::string>());
            }
        }
    }


}} // namespace protean::detail
