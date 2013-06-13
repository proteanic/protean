//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/xml_reader.hpp>
#include <protean/detail/xml_default_handler.hpp>
#include <protean/detail/xml_preserve_handler.hpp>
#include <protean/detail/xml_utility.hpp>
#include <protean/detail/scoped_xmlch.hpp>

#if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable:4512)
#endif
#include <boost/scope_exit.hpp>
#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/foreach.hpp>

namespace protean {

    xml_reader::xml_reader(std::istream& is, int mode) :
        m_is(is),
        m_mode(mode),
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
        boost::scoped_ptr<detail::xml_handler_base> handler;
        if ((m_mode & xml_mode::Preserve)!=0)
        {
            handler.reset(new detail::xml_preserve_handler(result, m_mode));
        }
        else
        {
            handler.reset(new detail::xml_default_handler(result, m_mode, m_factory));
        }
        
        try
        {
            xercesc::XMLPlatformUtils::Initialize();

            // Set up guards to correctly clean up parser and call terminate when
            // this scope ends. Xerces is picky and insists that the parser is deleted
            // before xercesc::XMLPlatformUtils::Terminate is called so the order of
            // guard creation (and hence destruction) is important.

#if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable:4100 4512)
#endif
            char terminate; // BOOST_SCOPE_EXIT will not accept an empty list under MSVC
            BOOST_SCOPE_EXIT ((&terminate))
            {
                xercesc::XMLPlatformUtils::Terminate();
            }
            BOOST_SCOPE_EXIT_END
#if defined(_MSC_VER)
#    pragma warning(pop)
#endif
            boost::scoped_ptr<xercesc::SAX2XMLReaderImpl> parser( new xercesc::SAX2XMLReaderImpl() );

            parser->setContentHandler(handler.get());
            parser->setErrorHandler(handler.get());
            parser->setPSVIHandler(handler.get());
            if((m_mode & xml_mode::Preserve)!=0)
            {
                parser->setLexicalHandler(handler.get());
            }
            
            boost::scoped_ptr<detail::xml_stream_resolver> resolver(new detail::xml_stream_resolver(m_entity_path));
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
                detail::scoped_xmlch schema( xercesc::XMLString::transcode(m_external_schema.c_str()) );
                parser->setProperty( xercesc::XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation, schema.get() );
            }

            detail::xml_stream_source source(m_is);
            parser->parse(source);
        }
        catch (const xercesc::XMLException& e)
        {
            boost::throw_exception(variant_error(std::string("Xerces exception: ") + detail::xml_utility::transcode(e.getMessage())));
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
    
    /*static*/ void xml_reader::initialiseXerces()
    {
        xercesc::XMLPlatformUtils::Initialize();
    }

    /*static*/ void xml_reader::shutdownXerces()
    {
        xercesc::XMLPlatformUtils::Terminate();
    }

    xml_reader& operator>>(xml_reader& reader, variant& variant)
    {
        reader.read(variant);
        return reader;
    }
} // namespace protean

