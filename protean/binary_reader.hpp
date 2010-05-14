//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_BINARY_READER_HPP
#define PROTEAN_BINARY_READER_HPP

#include <protean/config.hpp>

#include <protean/binary_common.hpp>
#include <protean/variant.hpp>

#if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable:4251)
#endif

namespace protean {

    class object_factory;
    
    class PROTEAN_DECL binary_reader
    {
    public:
        binary_reader(std::istream &is, int mode=binary_mode::Default);
        ~binary_reader();

        void read(variant& value);
        void read(std::string& value);
        void read(bool& value);
        void read(boost::int32_t& value);
        void read(boost::uint32_t& value);
        void read(boost::int64_t& value);
        void read(boost::uint64_t& value);
        void read(float &value);
        void read(double &value);
        void read(variant::date_t& value);
        void read(variant::time_t& value);
        void read(variant::date_time_t& value);
        void read(void*& data, size_t length);

        // read from stream
        void read_bytes(char* value, size_t length);
        void read_value(variant::enum_type_t type, variant& value);

        void set_factory(object_factory& factory);

    private:
        std::istream&                           m_is;
        boost::iostreams::filtering_istream     m_filter;
        int                                     m_mode;
        object_factory*                         m_factory;
        boost::uint16_t                         m_major_version;
        boost::uint16_t                         m_minor_version;

        void setup();
        void close();

        friend PROTEAN_DECL binary_reader& operator>>(binary_reader& reader, variant& v);
    };

} // namespace protean

#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#endif // PROTEAN_BINARY_READER_HPP
