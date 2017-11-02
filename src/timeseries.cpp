//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/detail/timeseries.hpp>
#include <protean/detail/timeseries_iterator.hpp>
#include <protean/variant.hpp>
#include <protean/variant_error.hpp>

#include <protean/detail/hash.hpp>

namespace protean { namespace detail {

    int timeseries::compare(const collection& rhs) const
    {
        const timeseries* cast_rhs = dynamic_cast<const timeseries*>(&rhs);
        if (cast_rhs==nullptr)
        {
            boost::throw_exception(variant_error("Unable to cast collection to timeseries"));
        }
        return (std::lexicographical_compare(m_value.begin(), m_value.end(), cast_rhs->m_value.begin(), cast_rhs->m_value.end()) ?
            -1 : (std::lexicographical_compare(cast_rhs->m_value.begin(), cast_rhs->m_value.end(), m_value.begin(), m_value.end()) ? 1 : 0));
    }

    boost::uint64_t timeseries::hash(boost::uint64_t seed) const
    {
        return hash_range(m_value.begin(), m_value.end(), seed);
    }

    bool timeseries::empty() const
    {
        return m_value.empty();
    }
    size_t timeseries::size() const
    {
        return m_value.size();
    }
    void timeseries::clear()
    {
        m_value.clear();
    }
    variant& timeseries::push_back(const date_time_t& time, const variant& value)
    {
        m_value.push_back( std::make_pair(time, value) );
        return m_value.rbegin()->second;
    }
    variant_const_iterator_base* timeseries::begin() const
    {
        return new timeseries_const_iterator(m_value.begin());
    }
    variant_const_iterator_base* timeseries::end() const
    {
        return new timeseries_const_iterator(m_value.end());
    }
    variant_iterator_base* timeseries::begin()
    {
        return new timeseries_iterator(m_value.begin());
    }
    variant_iterator_base* timeseries::end()
    {
        return new timeseries_iterator(m_value.end());
    }

}} // namespace protean::detail
