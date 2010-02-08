//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DETAIL_DUMMY_ITERATOR_HPP
#define PROTEAN_DETAIL_DUMMY_ITERATOR_HPP

#include <protean/config.hpp>

namespace protean { namespace detail {

    template<
        typename ITERATOR_TRAITS
      , typename BASE=variant_iterator_interface<ITERATOR_TRAITS>
    >
    class PROTEAN_DLLEXPORT dummy_iterator_interface : public BASE
    {
        typedef typename ITERATOR_TRAITS::value_type& reference_type;
        typedef typename BASE::date_time_t date_time_t;

    public:
        dummy_iterator_interface()
        {
        }
        const std::string& key() const
        {
            boost::throw_exception(variant_error("Attempt to dereference dummy iterator"));
        }
        reference_type value() const
        {
            boost::throw_exception(variant_error("Attempt to dereference dummy iterator"));
        }
        const date_time_t& time() const
        {
            boost::throw_exception(variant_error("Attempt to dereference dummy iterator"));
        }
        void increment()
        {
            boost::throw_exception(variant_error("Attempt to increment dummy iterator"));
        }
        void decrement()
        {
            boost::throw_exception(variant_error("Attempt to decrement dummy iterator"));
        }
        bool equal(const variant_const_iterator_base *rhs) const
        {
            const dummy_iterator_interface<const_iterator_traits>* cast_rhs =
                dynamic_cast<const dummy_iterator_interface<const_iterator_traits>*>(rhs);

            if (cast_rhs==NULL)
            {
                boost::throw_exception(variant_error("Unable to convert iterator to dummy iterator"));
            }
            return true;
        }
        BASE* clone()
        {
            return new dummy_iterator_interface<ITERATOR_TRAITS>();
        }
        variant_const_iterator_base* to_const() const
        {
            return new dummy_iterator_interface<const_iterator_traits>();
        }
    };

    typedef dummy_iterator_interface<const_iterator_traits> dummy_const_iterator;
    typedef dummy_iterator_interface<iterator_traits> dummy_iterator;

}} // namespace protean::detail

#endif // PROTEAN_DETAIL_DUMMY_ITERATOR_HPP
