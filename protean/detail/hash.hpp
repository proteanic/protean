//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DETAIL_HASH_HPP
#define PROTEAN_DETAIL_HASH_HPP

#include <boost/functional/hash.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace boost
{
    namespace posix_time
    {
        size_t hash_value(const time_duration& dur);
        size_t hash_value(const ptime& ptime);
    }
    
    namespace gregorian
    {
        size_t hash_value(const date& date);
    }
}

#endif // PROTEAN_DETAIL_HASH_HPP
