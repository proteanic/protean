//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_XML_READER_HPP
#define PROTEAN_XML_READER_HPP

#include <protean/config.hpp>
#include <protean/xml_common.hpp>

#include <map>
#include <string>

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable:4512 4251)
#endif

namespace protean {

    class object_factory;
    class variant;

    class PROTEAN_DECL xml_reader
    {
    public:
        typedef std::map<std::string, std::istream*> entity_stream_map_t;

    public:
        xml_reader(std::istream& is, int mode=xml_mode::None);
        void read(variant& result);
        void set_entity_path(const std::string& entity_path);
        void set_external_schema(const std::string& externalSchema);
        void add_entity_stream(const std::string& id, std::istream& is);
        void set_factory(object_factory& factory);

        // If you need to perform many xml reads then the performance impact of initialising the xerces library each
        // time may be significant, in which case the following functions can be used to explicitly initialise
        // the library, mitigating the cost.
        static void initialiseXerces();
        static void shutdownXerces();

    private:
        std::istream&       m_is;
        int                 m_mode;
        std::string         m_entity_path;
        std::string         m_external_schema;
        object_factory*     m_factory;

        entity_stream_map_t m_entities;

        friend PROTEAN_DECL xml_reader& operator>>(xml_reader& reader, variant& variant);
    };

} // namespace protean

#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#endif // PROTEAN_XML_READER_HPP
