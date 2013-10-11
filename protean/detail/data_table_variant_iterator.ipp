//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <algorithm>

namespace protean { namespace detail {

    /* Column iterator implementations */
    /***********************************/
    template <typename T, typename IteratorTraits>
    data_table_column_variant_iterator_interface<T, IteratorTraits>::
    data_table_column_variant_iterator_interface(const source_iterator_type& iterator)
        : m_iterator(iterator)
    {}

    template <typename T, typename IteratorTraits>
    const std::string& data_table_column_variant_iterator_interface<T, IteratorTraits>::key() const
    {
        boost::throw_exception(variant_error("Attempt to call key() on data table column variant iterator"));
    }

    template <typename T, typename IteratorTraits>
    //const typename data_table_column_variant_iterator_interface<T, IteratorTraits>::base::date_time_t&
    const typename data_table_column_variant_iterator_interface<T, IteratorTraits>::date_time_t&
    data_table_column_variant_iterator_interface<T, IteratorTraits>::time() const
    {
        boost::throw_exception(variant_error("Attempt to call time() on data table column variant iterator"));
    }

    template <typename T, typename IteratorTraits>
    typename data_table_column_variant_iterator_interface<T, IteratorTraits>::reference
    data_table_column_variant_iterator_interface<T, IteratorTraits>::value() const
    {
        make_copy<T>();
        return m_copy;
    }

    template <typename T, typename IteratorTraits>
    void data_table_column_variant_iterator_interface<T, IteratorTraits>::increment()
    {
        ++m_iterator;
    }

    template <typename T, typename IteratorTraits>
    void data_table_column_variant_iterator_interface<T, IteratorTraits>::decrement()
    {
        --m_iterator;
    }

    template <typename T, typename IteratorTraits>
    bool data_table_column_variant_iterator_interface<T, IteratorTraits>::equal(const variant_const_iterator_base* rhs) const
    {
        const data_table_column_variant_iterator_interface<T>* cast_rhs =
            dynamic_cast<const data_table_column_variant_iterator_interface<T>*>(rhs);

        if (cast_rhs == 0)
            boost::throw_exception(variant_error("Unable to convert iterator to data table column variant iterator"));

        return m_iterator == cast_rhs->m_iterator;
    }

    template <typename T, typename IteratorTraits>
    typename data_table_column_variant_iterator_interface<T, IteratorTraits>::base*
    data_table_column_variant_iterator_interface<T, IteratorTraits>::clone()
    {
        return new data_table_column_variant_iterator_interface(m_iterator);
    }

    template <typename T, typename IteratorTraits>
    variant_const_iterator_base* data_table_column_variant_iterator_interface<T, IteratorTraits>::to_const() const
    {
        return new data_table_column_variant_iterator_interface<T, const_iterator_traits>(m_iterator);
    }


    /* Row iterator implementations */
    /********************************/
    template <
        typename IteratorTraits,
        typename Base
    >
    data_table_variant_iterator_interface<IteratorTraits, Base>::
    data_table_variant_iterator_interface(const column_iterator_container& column_iterators)
        : m_column_iterators(column_iterators)
    {}

    template <
        typename IteratorTraits,
        typename Base
    >
    const std::string& data_table_variant_iterator_interface<IteratorTraits, Base>::key() const
    {
        boost::throw_exception(variant_error("Attempt to call key() on data table variant iterator"));
    }

    template <
        typename IteratorTraits,
        typename Base
    >
    const typename Base::date_time_t& data_table_variant_iterator_interface<IteratorTraits, Base>::time() const
    {
        boost::throw_exception(variant_error("Attempt to call time() on data table"));
    }

    template <
        typename IteratorTraits,
        typename Base
    >
    typename data_table_variant_iterator_interface<IteratorTraits, Base>::reference
    data_table_variant_iterator_interface<IteratorTraits, Base>::value() const
    {
        variant tuple(variant_base::Tuple, m_column_iterators.size());

        for (size_t i = 0; i < m_column_iterators.size(); ++i)
            tuple.at(i) = *(m_column_iterators[i]);

        m_copy = tuple;
        return m_copy;
    }

    template <
        typename IteratorTraits,
        typename Base
    >
    void data_table_variant_iterator_interface<IteratorTraits, Base>::increment()
    {
        for (column_iterator_container::iterator iter = m_column_iterators.begin()
                                               ; iter != m_column_iterators.end()
                                               ; ++iter)
            ++(*iter);
    }

    template <
        typename IteratorTraits,
        typename Base
    >
    void data_table_variant_iterator_interface<IteratorTraits, Base>::decrement()
    {
        for (column_iterator_container::iterator iter = m_column_iterators.begin()
                                               ; iter != m_column_iterators.end()
                                               ; ++iter)
            --(*iter);
    }

    template <
        typename IteratorTraits,
        typename Base
    >
    bool data_table_variant_iterator_interface<IteratorTraits, Base>::equal(const variant_const_iterator_base* rhs) const
    {
        const data_table_variant_iterator_interface<const_iterator_traits>* cast_rhs =
            dynamic_cast<const data_table_variant_iterator_interface<const_iterator_traits>*>(rhs);

        if (cast_rhs == 0)
            boost::throw_exception(variant_error("Unable to convert iterator to data table variant iterator"));

        if (m_column_iterators.size() != cast_rhs->m_column_iterators.size())
            boost::throw_exception(variant_error("Unable to compare variant iterators of two DataTables that have a different number of columns"));

        for (size_t i = 0; i < m_column_iterators.size(); ++i)
            if (m_column_iterators[i] != cast_rhs->m_column_iterators[i])
                return false;

        return true;
    }

    template <
        typename IteratorTraits,
        typename Base
    >
    Base* data_table_variant_iterator_interface<IteratorTraits, Base>::clone()
    {
        return new data_table_variant_iterator_interface(m_column_iterators);
    }

    template <
        typename IteratorTraits,
        typename Base
    >
    variant_const_iterator_base* data_table_variant_iterator_interface<IteratorTraits, Base>::to_const() const
    {
        typedef typename data_table_variant_iterator_interface<const_iterator_traits>::column_iterator_container
            const_column_iterator_container;

        const_column_iterator_container const_column_iterators(m_column_iterators.size());
        std::copy(m_column_iterators.begin(), m_column_iterators.end(), const_column_iterators.begin());

        return new data_table_variant_iterator_interface<const_iterator_traits>(const_column_iterators);
    }

}} // namespace protean::detail
