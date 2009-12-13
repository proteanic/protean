//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_BINARY_WRITER_HPP
#define PROTEAN_BINARY_WRITER_HPP

#include <protean/config.hpp>

#include <protean/binary_defs.hpp>
#include <protean/variant.hpp>

#include <boost/iostreams/filtering_stream.hpp>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4251)
#endif

namespace protean {
    
    class PROTEAN_DLLEXPORT binary_writer
    {
    public:
        enum enum_flag_t { None=0, Compress=1 };

    public:
        binary_writer(std::ostream &os, int flags=None);
        ~binary_writer();
        
        void write(const variant& value);
        void write(const std::string& value);
        void write(bool value);
        void write(variant::int32_t value);
        void write(variant::uint32_t value);
        void write(variant::int64_t value);
        void write(variant::uint64_t value);
        void write(float value);
        void write(double value);
        void write(const variant::date_t& value);
        void write(const variant::time_t& value);
        void write(const variant::date_time_t& value);
        void write(const void* data, size_t length);

        // write to stream
        void write_bytes(const char* value, size_t length);
        void write_value(const variant& value);

    private:
        std::ostream&                        m_os;
        boost::iostreams::filtering_ostream    m_filter;
        int                                    m_flags;

        void setup();
        void close();

        friend PROTEAN_DLLEXPORT binary_writer& operator<<(binary_writer& writer, const variant& v);
    };

} // namespace protean

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // PROTEAN_BINARY_WRITER_HPP
