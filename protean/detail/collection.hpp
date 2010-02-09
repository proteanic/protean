//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DETAIL_COLLECTION_HPP
#define PROTEAN_DETAIL_COLLECTION_HPP

#include <protean/config.hpp>

#include <protean/variant_iterator.hpp>

namespace protean { namespace detail {

    class PROTEAN_DECL collection
    {
    public:
        virtual ~collection() { }

        virtual int compare(const collection& rhs) const = 0;
        virtual size_t hash() const = 0;

        virtual bool empty() const = 0;
        virtual size_t size() const = 0;
        virtual void clear() = 0;

        virtual variant_const_iterator_base* begin() const = 0;
        virtual variant_const_iterator_base* end() const = 0;
        virtual variant_iterator_base* begin() = 0;
        virtual variant_iterator_base* end() = 0;
    };

}} // namespace protean::detail

#endif // PROTEAN_DETAIL_COLLECTION_HPP
