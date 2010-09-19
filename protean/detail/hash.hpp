//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DETAIL_HASH_HPP
#define PROTEAN_DETAIL_HASH_HPP

#include <boost/cstdint.hpp>
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <protean/detail/crc64.hpp>

namespace protean { 

class variant;

namespace detail
{
    inline boost::uint64_t hash_value(const variant& value, boost::uint64_t seed);

    inline boost::uint64_t hash_value(const unsigned char* value, size_t length, boost::uint64_t seed)
    {
        return update_crc64(seed, value, length);
    }

    inline boost::uint64_t hash_value(const std::string& value, boost::uint64_t seed)
    {
        return update_crc64(seed, (const unsigned char*)value.c_str(), value.size());
    }

    inline boost::uint64_t hash_value(const char* value, boost::uint64_t seed)
    {
        return update_crc64(seed, (const unsigned char*)value, strlen(value));
    }

    inline boost::uint64_t hash_value(bool value, boost::uint64_t seed)
    {
        return update_crc64(seed, (const unsigned char*)&value, 1);
    }

    inline boost::uint64_t hash_value(boost::int32_t value, boost::uint64_t seed)
    {
        return update_crc64(seed, (const unsigned char*)&value, 4);
    }

    inline boost::uint64_t hash_value(boost::uint32_t value, boost::uint64_t seed)
    {
        return update_crc64(seed, (const unsigned char*)&value, 4);
    }

    inline boost::uint64_t hash_value(boost::int64_t value, boost::uint64_t seed)
    {
        return update_crc64(seed, (const unsigned char*)&value, 8);
    }

    inline boost::uint64_t hash_value(boost::uint64_t value, boost::uint64_t seed)
    {
        return update_crc64(seed, (const unsigned char*)&value, 8);
    }

    inline boost::uint64_t hash_value(float value, boost::uint64_t seed)
    {
        return update_crc64(seed, (const unsigned char*)&value, 4);
    }

    inline boost::uint64_t hash_value(double value, boost::uint64_t seed)
    {
        return update_crc64(seed, (const unsigned char*)&value, 8);
    }

    inline boost::uint64_t hash_value(const boost::posix_time::time_duration& value, boost::uint64_t seed)
    {
        return hash_value((boost::int64_t)value.total_milliseconds(), seed);
    }

    inline boost::uint64_t hash_value(const boost::gregorian::date& value, boost::uint64_t seed)
    {
        static boost::gregorian::date min_date(boost::gregorian::min_date_time);
        return hash_value((boost::uint32_t)(value - min_date).days(), seed);
    }

    inline boost::uint64_t hash_value(const boost::posix_time::ptime& value, boost::uint64_t seed)
    {
        seed = hash_value(value.date(), seed);
        seed = hash_value(value.time_of_day(), seed);

        return seed;
    }

    template <class A, class B>
    inline boost::uint64_t hash_value(std::pair<A, B> const& value, boost::uint64_t seed)
    {
        seed = hash_value(value.first, seed);
        seed = hash_value(value.second, seed);

        return seed;
    }

    template <class It>
    inline boost::uint64_t hash_range(It first, It last, boost::uint64_t seed)
    {
        for(; first != last; ++first)
        {
            seed = hash_value(*first, seed);
        }
        return seed;
    }


}} // protean::detail

#endif // PROTEAN_DETAIL_HASH_HPP
