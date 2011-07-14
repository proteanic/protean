//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/array_iterator.hpp>
#include <protean/variant.hpp>

namespace protean {


    template<typename ITERATOR_TRAITS>
    array_iterator<ITERATOR_TRAITS>::array_iterator() :
        m_ref()
    {
    }

    template<typename ITERATOR_TRAITS>
    array_iterator<ITERATOR_TRAITS>::array_iterator(iterator_type iterator, variant_base::enum_type_t type) :
        m_ref(iterator, type)
    {
    }

    // Copy constructor specialisation
    template<typename ITERATOR_TRAITS>
    array_iterator<ITERATOR_TRAITS>::array_iterator(const array_iterator<ITERATOR_TRAITS>& rhs) :
        m_ref(rhs.m_ref)
    {
    }

    // Assignment operator specialisation
    template<typename ITERATOR_TRAITS>
    const array_iterator<ITERATOR_TRAITS>& array_iterator<ITERATOR_TRAITS>::operator=(const array_iterator<ITERATOR_TRAITS>& rhs)
    {
        m_ref = rhs.m_ref;
        return *this;
    }

    template<typename ITERATOR_TRAITS>
    bool array_iterator<ITERATOR_TRAITS>::equal(const array_iterator<array_const_iterator_traits>& rhs) const
    {
        return m_ref.m_value==rhs.m_ref.m_value;
    }

    template<typename ITERATOR_TRAITS>
    void array_iterator<ITERATOR_TRAITS>::increment()
    {
        if (!m_ref)
        {
            boost::throw_exception(variant_error("Attempt to access invalid iterator"));
        }
        ++m_ref.m_value;
    }

    template<typename ITERATOR_TRAITS>
    void array_iterator<ITERATOR_TRAITS>::decrement()
    {
        if (!m_ref)
        {
            boost::throw_exception(variant_error("Attempt to access invalid iterator"));
        }
        --m_ref.m_value;
    }

    template<typename ITERATOR_TRAITS>
    typename ITERATOR_TRAITS::value_type & array_iterator<ITERATOR_TRAITS>::dereference() const
    {
        if (!m_ref)
        {
            boost::throw_exception(variant_error("Attempt to access invalid iterator"));
        }
        return m_ref;
    }

    template<typename ITERATOR_TRAITS>
    void array_iterator<ITERATOR_TRAITS>::advance(difference_type n)
    {
        m_ref.m_value += n;
    }

    // Explicit template instantiation
    template class array_iterator<array_const_iterator_traits>;
    template class array_iterator<array_iterator_traits>;


} // namespace protean
