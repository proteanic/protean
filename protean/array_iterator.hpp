//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_ARRAY_ITERATOR_HPP
#define PROTEAN_ARRAY_ITERATOR_HPP

#include <protean/config.hpp>
#include <protean/variant_base.hpp>
#include <protean/variant_ref.hpp>

#include <boost/iterator/iterator_facade.hpp>

namespace protean {

    struct array_const_iterator_traits
    {
        typedef const variant_cref      value_type;
        typedef const variant_base*     iterator_type;
        typedef size_t                  difference_type;
    };

    struct array_iterator_traits
    {
        typedef const variant_ref       value_type;
        typedef variant_base*           iterator_type;
        typedef size_t                  difference_type;
    };

    template <typename ITERATOR_TRAITS>
    class PROTEAN_DECL array_iterator :
        public boost::iterator_facade<
                array_iterator<ITERATOR_TRAITS>
              , typename ITERATOR_TRAITS::value_type
              , boost::random_access_traversal_tag
            >
    {
        typedef typename ITERATOR_TRAITS::iterator_type iterator_type;

    public:
        // BOOST_FOREACH for some reason demands that this be public, possibly
        // because it has failed to figure out the type based on the traits,
        // which might in turn be because they are somewhat non-standard 
        // (e.g. iterator_category and pointer are missing, see 
        // http://www.sgi.com/tech/stl/iterator_traits.html).

        typedef typename ITERATOR_TRAITS::difference_type difference_type;

        array_iterator();
        array_iterator(iterator_type iterator, variant_base::enum_type_t type);

        template <typename OTHER_TRAITS>
        array_iterator(const array_iterator<OTHER_TRAITS>& rhs);

        array_iterator(const array_iterator& rhs);

        template <typename OTHER_TRAITS>
        const array_iterator& operator=(const array_iterator<OTHER_TRAITS>& rhs);

        const array_iterator& operator=(const array_iterator& rhs);

     private:
        friend class boost::iterator_core_access;
        friend class variant;

        template <class> friend class array_iterator;

        void increment();

        void decrement();

        bool equal(const array_iterator<array_const_iterator_traits>& other) const;

        typename ITERATOR_TRAITS::value_type & dereference() const;

        void advance(difference_type n);

    private:
        typename boost::remove_const<typename ITERATOR_TRAITS::value_type>::type m_ref;
    };

    // Copy constructor definitions
    template<typename ITERATOR_TRAITS>
    template<typename OTHER_TRAITS>
    array_iterator<ITERATOR_TRAITS>::array_iterator(const array_iterator<OTHER_TRAITS>& rhs) :
        m_ref(rhs.m_ref)
    {
    }

    // Assignment operator definitions
    template<typename ITERATOR_TRAITS>
    template<typename OTHER_TRAITS>
    const array_iterator<ITERATOR_TRAITS>& array_iterator<ITERATOR_TRAITS>::operator=(const array_iterator<OTHER_TRAITS>& rhs)
    {
        m_ref = rhs.m_ref;
        return *this;
    }

} // protean

#endif // PROTEAN_ARRAY_ITERATOR_HPP
