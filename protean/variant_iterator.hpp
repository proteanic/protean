//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_VARIANT_ITERATOR_HPP
#define PROTEAN_VARIANT_ITERATOR_HPP

#include <protean/config.hpp>

#include <vector>
#include <list>
#include <map>
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4251)
#endif

namespace protean {

    class variant;
    struct const_iterator_traits;
    struct iterator_traits;

    template <typename ITERATOR_TRAITS>
    class PROTEAN_DLLEXPORT variant_iterator_interface
    {
    public:
        typedef boost::posix_time::ptime date_time_t;
        typedef typename ITERATOR_TRAITS::value_type& reference_type;

    public:
        virtual ~variant_iterator_interface() { }
        
        virtual const std::string& key() const = 0;
        virtual reference_type value() const = 0;
        virtual const date_time_t& time() const = 0;
        virtual void increment() = 0;
        virtual void decrement() = 0;
        virtual bool equal(const variant_iterator_interface *rhs) const = 0;
        virtual variant_iterator_interface* clone() = 0;
        virtual variant_iterator_interface<const_iterator_traits>* to_const() const = 0;
    };

    typedef variant_iterator_interface<const_iterator_traits> variant_const_iterator_base;
    typedef variant_iterator_interface<iterator_traits> variant_iterator_base;

    template <typename ITERATOR_TRAITS>
    class PROTEAN_DLLEXPORT variant_iterator :
        public boost::iterator_facade<
                variant_iterator<ITERATOR_TRAITS>
              , typename ITERATOR_TRAITS::value_type
              , boost::bidirectional_traversal_tag
            >
    {
        typedef typename ITERATOR_TRAITS::value_type& reference_type;
        typedef typename variant_iterator_interface<ITERATOR_TRAITS>::date_time_t date_time_t;

    public:

        // BOOST_FOREACH for some reason demands that this be public, possibly
        // because it has failed to figure out the type based on the traits,
        // which might in turn be because they are somewhat non-standard 
        // (e.g. iterator_category and pointer are missing, see 
        // http://www.sgi.com/tech/stl/iterator_traits.html).
        typedef typename ITERATOR_TRAITS::difference_type difference_type;

        variant_iterator();
        variant_iterator(variant_iterator_interface<ITERATOR_TRAITS> * iterator);

        template <typename OTHER_TRAITS>
        variant_iterator(const variant_iterator<OTHER_TRAITS>& rhs);

        variant_iterator(const variant_iterator& rhs);

        template <typename OTHER_TRAITS>
        const variant_iterator& operator=(const variant_iterator<OTHER_TRAITS>& rhs);

        const variant_iterator& operator=(const variant_iterator& rhs);

        const std::string& key() const;

        reference_type value() const;

        const date_time_t& time() const;

    private:
        friend class boost::iterator_core_access;
        friend class variant;
        template <class> friend class variant_iterator;

        void increment();

        void decrement();

        bool equal(const variant_iterator<ITERATOR_TRAITS>& other) const;

        reference_type dereference() const;

        void advance(difference_type n);

        const variant_iterator_interface<ITERATOR_TRAITS>* iterator() const;


        boost::shared_ptr<variant_iterator_interface<ITERATOR_TRAITS> > m_iterator;
    };

    // Copy constructor definitions
    template<typename ITERATOR_TRAITS>
    template<typename OTHER_TRAITS>
    variant_iterator<ITERATOR_TRAITS>::variant_iterator(const variant_iterator<OTHER_TRAITS>& rhs)
        : m_iterator( rhs.iterator()->to_const() )
    {
    }

    // Assignment operator definitions
    template<typename ITERATOR_TRAITS>
    template<typename OTHER_TRAITS>
    const variant_iterator<ITERATOR_TRAITS>& variant_iterator<ITERATOR_TRAITS>::operator=(const variant_iterator<OTHER_TRAITS>& rhs)
    {
        m_iterator.reset( rhs.iterator()->to_const() );
        return *this;
    }

} // protean

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // PROTEAN_VARIANT_ITERATOR_HPP
