//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/detail/hash.hpp>

namespace boost
{
    namespace posix_time
    {
        size_t hash_value(const time_duration &dur)
        {
            size_t seed = 0;
            if(dur.is_special())
            {
                if(dur.is_neg_infinity())
                    seed = -1;
                else if(dur.is_pos_infinity())
                    seed = 1;
                else if(dur.is_not_a_date_time())
                    seed = 0;
            } 
            else
                hash_combine(seed,dur.total_nanoseconds());
            return seed;
        }
        
        size_t hash_value(const ptime &ptime)
        {
            size_t seed = 0;
            hash_combine(seed,ptime.date());
            hash_combine(seed,ptime.time_of_day());
            return seed;
        }
    }
    
    namespace gregorian
    {
        size_t hash_value(const date &date)
        {
            size_t seed = 0;
            if(date.is_special())
            {
                if(date.is_neg_infinity())
                    seed = -2;
                else if(date.is_pos_infinity())
                    seed = 2;
                else if(date.is_not_a_date())
                    seed = 0;
            }
            else
            {
                date::ymd_type ymd = date.year_month_day();
                hash_combine(seed,ymd.year);
                hash_combine(seed,ymd.month);
                hash_combine(seed,ymd.day);
            }
            return seed; 
        }
    }
} 
