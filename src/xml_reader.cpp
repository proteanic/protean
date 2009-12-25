//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/xml_reader.hpp>
#include <protean/xml_parser.hpp>
#include <protean/detail/scoped_xmlch.hpp>

#include <boost/scope_exit.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/foreach.hpp>

namespace protean {

    xml_reader::xml_reader(std::istream &is, int flags) :
        m_is(is),
        m_flags(flags),
        m_entity_path("./"),
        m_factory(NULL)
    {
        if(!is.good())
        {
            boost::throw_exception(variant_error("Input stream is bad"));
        }
    }
    
    void xml_reader::read(variant& result)
    {
        sax_content_handler handler(result, m_flags, m_factory);
        
        try
        {
            xercesc::XMLPlatformUtils::Initialize();

            // Set up guards to correctly clean up parser and call terminate when
            // this scope ends. Xerces is picky and insists that the parser is deleted
            // before xercesc::XMLPlatformUtils::Terminate is called so the order of
            // guard creation (and hence destruction) is important.

            char terminate; // BOOST_SCOPE_EXIT will not accept an empty list under MSVC
            BOOST_SCOPE_EXIT ((&terminate))
            {
                xercesc::XMLPlatformUtils::Terminate();
            }
            BOOST_SCOPE_EXIT_END

            boost::scoped_ptr<xercesc::SAX2XMLReaderImpl> parser( new xercesc::SAX2XMLReaderImpl() );

            parser->setContentHandler( &handler );
            parser->setErrorHandler( &handler);
            parser->setPSVIHandler( &handler );
            if((m_flags & Preserve)!=0)
            {
                parser->setLexicalHandler(&handler);
            }
            
            boost::scoped_ptr<stream_resolver> resolver(new stream_resolver(m_entity_path));
            BOOST_FOREACH(const entity_stream_map_t::value_type& e, m_entities)
            {
                resolver->add_entity_stream(e.first, e.second);
            }
            parser->setEntityResolver(resolver.get());

            parser->setFeature( xercesc::XMLUni::fgSAX2CoreNameSpaces, true );
            parser->setFeature( xercesc::XMLUni::fgSAX2CoreValidation, true );
            parser->setFeature( xercesc::XMLUni::fgXercesDynamic, true );
            parser->setFeature( xercesc::XMLUni::fgXercesSchema, true );

            if (!m_external_schema.empty())
            {
		scoped_xmlch schema( xercesc::XMLString::transcode(m_external_schema.c_str()) );
                parser->setProperty( xercesc::XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation, schema.get() );
            }

            stream_input_source source(m_is);
            parser->parse(source);
        }
        catch (const xercesc::XMLException& e)
        {
            boost::throw_exception(variant_error(std::string("Xerces exception: ") + sax_content_handler::transcode(e.getMessage())));
        }
        catch (const std::exception& e)
        {
            boost::throw_exception(variant_error(e.what()));
        }
        catch (...)
        {
            boost::throw_exception(variant_error("Unhandled exception"));
        }
    }

    void xml_reader::set_entity_path(const std::string& entity_path)
    {
        m_entity_path = entity_path;
    }

    void xml_reader::set_external_schema(const std::string& external_schema)
    {
        m_external_schema = external_schema;
    }

    void xml_reader::add_entity_stream(const std::string& name, std::istream& is)
    {
        entity_stream_map_t::const_iterator it(m_entities.find(name));
        if (it!=m_entities.end())
        {
            boost::throw_exception(variant_error(std::string("Duplicate entity '") + name + "' passed to parser"));
        }
        m_entities.insert(make_pair(name, &is));
    }

    void xml_reader::set_factory(object_factory& factory)
    {
        m_factory = &factory;
    }

    xml_reader& operator>>(xml_reader& reader, variant& variant)
    {
        reader.read(variant);
        return reader;
    }
} // namespace protean
