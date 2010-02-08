//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/variant_iterator.hpp>
#include <protean/variant.hpp>

namespace protean {

    template<typename ITERATOR_TRAITS>
    variant_iterator<ITERATOR_TRAITS>::variant_iterator() :
        m_iterator()
    {
    }

    template<typename ITERATOR_TRAITS>
    variant_iterator<ITERATOR_TRAITS>::variant_iterator(variant_iterator_interface<ITERATOR_TRAITS>* iterator) :
        m_iterator(iterator)
    {
    }

    // Copy constructor specialisation
    template<typename ITERATOR_TRAITS>
    variant_iterator<ITERATOR_TRAITS>::variant_iterator(const variant_iterator<ITERATOR_TRAITS>& rhs) :
        m_iterator(rhs.m_iterator->clone())
    {
    }

    // Assignment operator specialisation
    template<typename ITERATOR_TRAITS>
    const variant_iterator<ITERATOR_TRAITS>& variant_iterator<ITERATOR_TRAITS>::operator=(const variant_iterator<ITERATOR_TRAITS>& rhs)
    {
        m_iterator.reset(rhs.m_iterator->clone());
        return *this;
    }

    template<typename ITERATOR_TRAITS>
    const std::string& variant_iterator<ITERATOR_TRAITS>::key() const
    {
        if (!m_iterator)
        {
            boost::throw_exception(variant_error("Attempt to access invalid iterator"));
        }
        return m_iterator->key();
    }

    template<typename ITERATOR_TRAITS>
    const typename variant_iterator<ITERATOR_TRAITS>::date_time_t& variant_iterator<ITERATOR_TRAITS>::time() const
    {
        if (!m_iterator)
        {
            boost::throw_exception(variant_error("Attempt to access invalid iterator"));
        }
        return m_iterator->time();
    }

    template<typename ITERATOR_TRAITS>
    typename variant_iterator<ITERATOR_TRAITS>::reference_type variant_iterator<ITERATOR_TRAITS>::value() const
    {
        if (!m_iterator)
        {
            boost::throw_exception(variant_error("Attempt to access invalid iterator"));
        }
        return m_iterator->value();
    }

    template<typename ITERATOR_TRAITS>
    void variant_iterator<ITERATOR_TRAITS>::increment()
    {
        if (!m_iterator)
        {
            boost::throw_exception(variant_error("Attempt to access invalid iterator"));
        }
        m_iterator->increment();
    }

    template<typename ITERATOR_TRAITS>
    void variant_iterator<ITERATOR_TRAITS>::decrement()
    {
        if (!m_iterator)
        {
            boost::throw_exception(variant_error("Attempt to access invalid iterator"));
        }
        m_iterator->decrement();
    }

    template<typename ITERATOR_TRAITS>
    bool variant_iterator<ITERATOR_TRAITS>::equal(const variant_iterator<ITERATOR_TRAITS>& other) const
    {
        if (!m_iterator || !other.m_iterator)
        {
            boost::throw_exception(variant_error("Attempt to access invalid iterator"));
        }

        return m_iterator->equal(other.m_iterator.get());
    }

    template<typename ITERATOR_TRAITS>
    typename variant_iterator<ITERATOR_TRAITS>::reference_type variant_iterator<ITERATOR_TRAITS>::dereference() const
    {
        if (!m_iterator)
        {
            boost::throw_exception(variant_error("Attempt to access invalid iterator"));
        }
        return m_iterator->value();
    }

    template<typename ITERATOR_TRAITS>
    void variant_iterator<ITERATOR_TRAITS>::advance(difference_type n)
    {
        if (n > 0)
        {
            while (n > 0)
            {
                --n;
                increment();
            }
        }
        else if (n < 0)
        {
            while (n < 0)
            {
                ++n;
                decrement();
            }
        }
    }

  
    template<typename ITERATOR_TRAITS>
    const variant_iterator_interface<ITERATOR_TRAITS> *
        variant_iterator<ITERATOR_TRAITS>::iterator() const
    {
        return m_iterator.get();
    }
  
    // Explicit template instantiation
    template class variant_iterator<const_iterator_traits>;
    template class variant_iterator<iterator_traits>;

} // namespace protean
