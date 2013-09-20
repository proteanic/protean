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

    // Forward declaration
    template <typename T>
    class data_table_column;


    /* Variant (dynamic) iterators *************************************************/
    /* Allow variant iteration along a DataTable (to satisfy Collection interface) */
    /* by creating a variant copy for each value in the DataTable                  */
    /*******************************************************************************/

    // Column iterator -- returns a variant copy for each value in the column
    template <typename T>
    class data_table_column_variant_iterator_interface
        : public variant_iterator_interface<const_iterator_traits>
    {
    private:
        typedef variant_iterator_interface<const_iterator_traits> base;

        typedef const_iterator_traits                             traits;
        typedef variant                                           variant_type;
        typedef typename data_table_column<T>::const_iterator     source_iterator_type;
        typedef typename traits::list_iterator_type               iterator_type;
        typedef typename traits::value_type&                      reference;

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
        void make_copy() const { m_copy = *m_iterator; }

        template <>
        void make_copy<detail::string>() const
        {
            m_copy = variant_type(variant_base::String);
            m_copy.m_value.set<variant_base::String>(*m_iterator);
        }

    private:
        source_iterator_type m_iterator;
        mutable variant_type m_copy;
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
        typedef variant_iterator<const_iterator_traits> column_iterator;
        typedef std::vector<column_iterator>            column_iterator_container;

        typedef variant                                 variant_type;
        typedef typename IteratorTraits::value_type&    reference;

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

    public:
        const column_iterator_container& column_iterators() const;

    private:
        column_iterator_container m_column_iterators;
        mutable variant_type m_copy;
    };

}} // namespace protean::detail

#include <protean/detail/data_table_variant_iterator.ipp>


#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#endif // PROTEAN_DETAIL_DATA_TABLE_VARIANT_ITERATOR_HPP
