//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DETAIL_TUPLE_HPP
#define PROTEAN_DETAIL_TUPLE_HPP

#include <protean/config.hpp>
#include <protean/detail/sequence.hpp>

namespace protean { namespace detail {

    class PROTEAN_DECL tuple : public sequence
    {
    public:
        tuple(size_t size);
        tuple(const tuple& rhs);
        tuple& operator=(const tuple& rhs);
        ~tuple();

        int compare(const collection& rhs) const;
        boost::uint64_t hash(boost::uint64_t seed) const;

        variant& at(size_t index);
        const variant& at(size_t index) const;

        size_t size() const;
        bool empty() const;
        void clear();

        void swap(tuple& rhs);

        variant_const_iterator_base* begin() const;
        variant_const_iterator_base* end() const;
        variant_iterator_base* begin();
        variant_iterator_base* end();

    private:
        boost::uint16_t m_size;
        variant*        m_value;
    };

}} // namespace protean::detail

#endif // PROTEAN_DETAIL_TUPLE_HPP
