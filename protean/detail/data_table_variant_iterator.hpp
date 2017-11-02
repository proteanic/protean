//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DETAIL_DATA_TABLE_VARIANT_ITERATOR_HPP
#define PROTEAN_DETAIL_DATA_TABLE_VARIANT_ITERATOR_HPP

#include <protean/config.hpp>
#include <protean/variant_error.hpp>
#include <protean/variant_base.hpp>
#include <protean/variant.hpp>

#if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable:4251)
#endif

namespace protean { namespace detail {
    template <typename I, typename U>
    struct MakeCopy {
        static void make_copy(variant &cpy, I it) {
            cpy = *it;
        }
    };

    template <typename I>
    struct MakeCopy<I, detail::string> {
        static void make_copy(variant &cpy, I it) {
            cpy = variant(variant_base::String);
            cpy.m_value.set<variant_base::String>(*it);
        }
    };

    // Forward declaration
    template <variant_base::enum_type_t E>
    class data_table_column;


    /* Variant (dynamic) iterators *************************************************/
    /* Allow variant iteration along a DataTable (to satisfy Collection interface) */
    /* by creating a variant copy for each value in the DataTable                  */
    /*******************************************************************************/

    // Column iterator -- returns a variant copy for each value in the column
    template <
          typename T
        , typename IteratorTraits = const_iterator_traits
    >
    class data_table_column_variant_iterator_interface
        : public variant_iterator_interface<IteratorTraits>
    {
    private:
        typedef IteratorTraits                           traits;
        typedef variant_iterator_interface<traits>       base;

        typedef typename base::date_time_t               date_time_type;
        typedef typename const_iterator_traits::template
            column_iterator_type<T>::type                source_iterator_type;
        typedef typename traits::list_iterator_type      iterator_type;
        typedef typename traits::value_type&             reference;

    public:
        data_table_column_variant_iterator_interface(const source_iterator_type& iterator);

        const std::string& key() const;
        const typename base::date_time_t& time() const;

        reference value() const;

        void increment();
        void decrement();

        bool equal(const variant_const_iterator_base* rhs) const;

        base* clone();
        variant_const_iterator_base* to_const() const;

    private:
        template <typename U>
        void make_copy() const { MakeCopy<source_iterator_type, U>::make_copy(m_copy, m_iterator); }

    private:
        source_iterator_type m_iterator;
        mutable variant      m_copy;

        template <class, class>
        friend class data_table_column_variant_iterator_interface;
    };

    // Variant specialization for variant fallback -- allows proper mutation
    template <
        typename IteratorTraits
    >
    class data_table_column_variant_iterator_interface<variant, IteratorTraits>
        : public variant_iterator_interface<IteratorTraits>
    {
    private:
        typedef IteratorTraits                             traits;
        typedef variant_iterator_interface<IteratorTraits> base;

        typedef typename base::date_time_t                 date_time_type;
        typedef typename traits::template
            column_iterator_type<variant>::type            source_iterator_type;
        typedef typename traits::list_iterator_type        iterator_type;
        typedef typename traits::value_type&               reference;

    public:
        data_table_column_variant_iterator_interface(const source_iterator_type& iterator);

        const std::string& key() const;
        const typename base::date_time_t& time() const;

        reference value() const;

        void increment();
        void decrement();

        bool equal(const variant_const_iterator_base* rhs) const;

        base* clone();
        variant_const_iterator_base* to_const() const;

    private:
        source_iterator_type m_iterator;

        template <class, class>
        friend class data_table_column_variant_iterator_interface;
    };

    // Row iterator -- returns a variant tuple (whose entries come from the variant
    // column iterators) for each row in the DataTable
    template <
        typename IteratorTraits,
        typename Base = variant_iterator_interface<IteratorTraits>
    >
    class data_table_variant_iterator_interface
        : public Base
    {
    private:
        typedef IteratorTraits traits;
        typedef variant_iterator<traits>     column_iterator;
        typedef std::vector<column_iterator> column_iterator_container;

        typedef typename traits::value_type& reference;

    public:
        data_table_variant_iterator_interface(const column_iterator_container& column_iterators);

        const std::string& key() const;
        const typename Base::date_time_t& time() const;

        reference value() const;

        void increment();
        void decrement();

        bool equal(const variant_const_iterator_base* rhs) const;

        Base* clone();
        variant_const_iterator_base* to_const() const;

    private:
        column_iterator_container m_column_iterators;
        mutable variant           m_copy;

        template <class, class>
        friend class data_table_variant_iterator_interface;
    };

}} // namespace protean::detail

#include <protean/detail/data_table_variant_iterator.ipp>


#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#endif // PROTEAN_DETAIL_DATA_TABLE_VARIANT_ITERATOR_HPP
