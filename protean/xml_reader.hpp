//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_XML_READER_HPP
#define PROTEAN_XML_READER_HPP

#include <protean/config.hpp>

#include <map>
#include <string>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4251)
#endif

namespace protean {

    class object_factory;
    class variant;

    class PROTEAN_DLLEXPORT xml_reader
    {
    public:
        enum enum_flag_t { None=0, Preserve=1, CreateProxy=2 };

        typedef std::map<std::string, std::istream*> entity_stream_map_t;

    public:
        xml_reader(std::istream& is, int flags=None);
        void read(variant& result);
        void set_entity_path(const std::string& entity_path);
        void set_external_schema(const std::string& externalSchema);
        void add_entity_stream(const std::string& id, std::istream& is);
        void set_factory(object_factory& factory);

    private:
        std::istream&        m_is;
        int                    m_flags;
        std::string            m_entity_path;
        std::string            m_external_schema;
        object_factory*        m_factory;

        entity_stream_map_t m_entities;

        friend PROTEAN_DLLEXPORT xml_reader& operator>>(xml_reader& reader, variant& variant);
    };

} // namespace protean

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // PROTEAN_XML_READER_HPP
