#include <protean/detail/xml_handler_base.hpp>

#ifndef PROTEAN_DISABLE_XML

#include <protean/detail/xml_utility.hpp>

#include <protean/detail/scoped_xmlch.hpp>

namespace protean { namespace detail {

    xml_stream_resolver::xml_stream_resolver(const std::string& entity_path) :
        m_entity_path(boost::filesystem::system_complete(boost::filesystem::path(entity_path)))
    { }

    xercesc::InputSource* xml_stream_resolver::resolveEntity(
        const XMLCh* const /*publicId*/,
        const XMLCh* const systemId )
    {
        xercesc::InputSource* result = nullptr;

        std::string name = xml_utility::transcode(systemId);
        entity_stream_map_t::iterator it(m_entities.find(name));
        if (it!=m_entities.end())
        {
            std::istream* stream = it->second;
            result = new xml_stream_source(*stream);
            m_entities.erase(it);
        }
        else
        {
            boost::filesystem::path full_path = boost::filesystem::absolute(boost::filesystem::path(name), m_entity_path);
            if (boost::filesystem::exists(full_path))
            {
                detail::scoped_xmlch full_path_str( xercesc::XMLString::transcode(full_path.string().c_str()) );
                result = new xercesc::LocalFileInputSource(full_path_str.get());
            }
        }
        return result;
    }

    void xml_stream_resolver::add_entity_stream(const std::string& name, std::istream* is)
    {
        entity_stream_map_t::const_iterator it(m_entities.find(name));
        if (it!=m_entities.end())
        {
            boost::throw_exception(variant_error(std::string("Duplicate entity stream ") + name + " passed to parser"));
        }
        m_entities.insert(make_pair(name, is));
    }

    xml_stream_source::xml_bin_stream::xml_bin_stream(std::istream& is) :
        m_is(is),
        m_pos(0)
    { }

    XMLFilePos xml_stream_source::xml_bin_stream::curPos() const
    {
        return m_pos;
    }
        
    XMLSize_t xml_stream_source::xml_bin_stream::readBytes( XMLByte *const toFill, const XMLSize_t maxToRead )
    {
        try
        {
            m_is.read(reinterpret_cast<char* const>(toFill), maxToRead);
            m_pos += m_is.gcount();
            return m_is.gcount();
        }
        catch(...)
        {
            boost::throw_exception(variant_error("Exception occurred whilst reading from stream"));
        }
    }

    const XMLCh* xml_stream_source::xml_bin_stream::getContentType() const
    {
        return 0;
    
    }

    xml_stream_source::xml_stream_source(std::istream& is) :
        m_is( is )
    {
        if(!is.good())
        {
            boost::throw_exception(variant_error("Input stream is bad"));
        }
    }

    xml_stream_source::~xml_stream_source()
    {
        m_is.clear();
    }

    xercesc::BinInputStream* xml_stream_source::makeStream() const 
    {
        return new xml_bin_stream(m_is);
    }

    xml_handler_base::xml_handler_base(variant& result, int mode) :
        m_result(result),
        m_mode(mode),
        m_locator(nullptr)
    { }

    xml_handler_base::~xml_handler_base()
    { }

    void xml_handler_base::setDocumentLocator(const xercesc::Locator* const locator)
    {
        m_locator = locator;
    }

    void xml_handler_base::warning(const xercesc::SAXParseException& e)
    {
        XMLFileLoc line_num = e.getLineNumber();
        std::string message(xml_utility::transcode(e.getMessage()));

        std::cerr << "XML parse warning: (line " << line_num << ") ";
        std::cerr << message << std::endl;
    }

    void xml_handler_base::error(const xercesc::SAXParseException& e)
    {
        XMLFileLoc line_num = e.getLineNumber();
        std::string message(xml_utility::transcode(e.getMessage()));

        if (line_num == 0)
        {
            boost::throw_exception(variant_error(message));
        }
        else
        {
            boost::throw_exception(variant_error((boost::format("line %d: %s") % static_cast<int>(line_num) % message).str()));
        }
    }

    void xml_handler_base::fatalError(const xercesc::SAXParseException& e)
    {
        XMLFileLoc line_num = e.getLineNumber();
        const std::string message(xml_utility::transcode(e.getMessage()));

        if (line_num == 0)
        {
            boost::throw_exception(variant_error(message));
        }
        else
        {
            boost::throw_exception(variant_error((boost::format("line %d: %s") % static_cast<int>(line_num) % message).str()));
        }
    }

}} // protean::detail

#endif // #ifndef PROTEAN_DISABLE_XML
