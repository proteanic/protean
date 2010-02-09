//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DETAIL_TIMESERIES_HPP
#define PROTEAN_DETAIL_TIMESERIES_HPP

#include <protean/config.hpp>

#include <protean/detail/collection.hpp>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4251)
#endif

namespace protean { namespace detail {

    class PROTEAN_DECL timeseries : public collection
    {
        typedef boost::posix_time::ptime date_time_t;
        typedef std::vector< std::pair<date_time_t, variant> > container_t;

    public:
        int compare(const collection& rhs) const;
        size_t hash() const;
        bool empty() const;
        size_t size() const;
        void clear();

        variant& push_back(const date_time_t& time, const variant& value);

        variant_const_iterator_base* begin() const;
        variant_const_iterator_base* end() const;
        variant_iterator_base* begin();
        variant_iterator_base* end();

    private:
        container_t m_value;
    };

}} // namespace protean::detail

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // PROTEAN_DETAIL_TIMESERIES_HPP
