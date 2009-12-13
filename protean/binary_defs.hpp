//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_BINARY_DEFS_HPP
#define PROTEAN_BINARY_DEFS_HPP

#include <protean/config.hpp>

#include <boost/iostreams/filter/zlib.hpp>

namespace protean {

    struct binary_compression_params : boost::iostreams::zlib_params
    {
        binary_compression_params() :
            boost::iostreams::zlib_params(
                boost::iostreams::zlib::default_compression,
                boost::iostreams::zlib::deflated,
                boost::iostreams::zlib::default_window_bits, 
                boost::iostreams::zlib::default_mem_level, 
                boost::iostreams::zlib::default_strategy,
                true)
        {}
    };

    typedef struct {
        unsigned short    year;
        unsigned char    month;
        unsigned char    day;
    } binary_date_t;

    typedef struct {
        unsigned char    hour;
        unsigned char    minute;
        unsigned char    second;
        unsigned char    _unused;
    } binary_time_t;

    static const boost::uint32_t binary_magic_number = 0x484913FF;
    static const boost::uint16_t binary_major_version = 1;
    static const boost::uint16_t binary_minor_version = 0;

} // namespace protean

#endif // PROTEAN_BINARY_DEFS_HPP
