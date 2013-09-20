//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <boost/preprocessor/seq/for_each.hpp>

namespace protean { namespace detail {

    /**************************/
    /* data_table_column_base */
    /**************************/

    /* Virtual templated method dispatchers */
    /****************************************/
    template <typename V>
    void data_table_column_base::push_back(const V& value)
    {
        return push_back_impl(value, static_cast<V*>(0));
    }

    template <typename V>
    typename std::vector<V>::const_iterator data_table_column_base::begin() const
    {
        return begin_impl(static_cast<V*>(0));
    }

    template <typename V>
    typename std::vector<V>::const_iterator data_table_column_base::end() const
    {
        return end_impl(static_cast<V*>(0));
    }

    template <typename V>
    typename std::vector<V>::iterator data_table_column_base::begin()
    {
        return begin_impl(static_cast<V*>(0));
    }

    template <typename V>
    typename std::vector<V>::iterator data_table_column_base::end()
    {
        return end_impl(static_cast<V*>(0));
    }


    /************************/
    /* data_table_column<T> */
    /************************/

    template <typename T>
    data_table_column<T>::
    data_table_column(const std::string& name, variant_base::enum_type_t type, size_t capacity /* = 0 */)
        : data_table_column_base(name, type)
    {
        if (capacity > 0)
            m_values.reserve(capacity);
    }

    template <typename T>
    data_table_column_base* data_table_column<T>::clone() const
    {
        return new data_table_column(*this);
    }

    template <typename T>
    void data_table_column<T>::resize(size_t n)
    {
        m_values.resize(n);
    }

    template <typename T>
    bool data_table_column<T>::empty() const
    {
        return m_values.empty();
    }

    template <typename T>
    size_t data_table_column<T>::size() const
    {
        return m_values.size();
    }

    template <typename T>
    void data_table_column<T>::clear()
    {
        m_values.clear();
    }

    template <typename T>
    variant_const_iterator_base* data_table_column<T>::begin() const
    {
        return new data_table_column_variant_iterator_interface<T>(m_values.begin());
    }

    template <typename T>
    variant_const_iterator_base* data_table_column<T>::end() const
    {
        return new data_table_column_variant_iterator_interface<T>(m_values.end());
    }

    template <typename T>
    variant_const_iterator_base* data_table_column<T>::begin()
    {
        const data_table_column<T>& const_this = *this;
        return const_this.begin();
    }

    template <typename T>
    variant_const_iterator_base* data_table_column<T>::end()
    {
        const data_table_column<T>& const_this = *this;
        return const_this.end();
    }

    /* Typed method implementation template-dispatchers */
    /****************************************************/
    #define PROTEAN_DETAIL_DATA_TABLE_COLUMN_DEFINE_VIRTUAL_IMPLS(r, data, type)                       \
        template <typename T>                                                                          \
        void data_table_column<T>::push_back_impl(const type& value, type* p /* = 0 */)                \
        { return push_back_impl_tmpl<type>(value, p); }                                                \
                                                                                                       \
        template <typename T>                                                                          \
        std::vector<type>::const_iterator data_table_column<T>::begin_impl(type* p /* = 0 */) const    \
        { return begin_impl_tmpl<type>(p); }                                                           \
                                                                                                       \
        template <typename T>                                                                          \
        std::vector<type>::const_iterator data_table_column<T>::end_impl(type* p /* = 0 */) const      \
        { return end_impl_tmpl<type>(p); }                                                             \
                                                                                                       \
        template <typename T>                                                                          \
        std::vector<type>::iterator data_table_column<T>::begin_impl(type* p /* = 0 */)                \
        { return begin_impl_tmpl<type>(p); }                                                           \
                                                                                                       \
        template <typename T>                                                                          \
        std::vector<type>::iterator data_table_column<T>::end_impl(type* p /* = 0 */)                  \
        { return end_impl_tmpl<type>(p); }

    BOOST_PP_SEQ_FOR_EACH(PROTEAN_DETAIL_DATA_TABLE_COLUMN_DEFINE_VIRTUAL_IMPLS, , PROTEAN_DETAIL_DATA_TABLE_COLUMN_SUPPORTED_TYPES)

    /* Typed method template implementations */
    /*****************************************/

    // void push_back()
    template <typename T>
        template <typename V>
        void data_table_column<T>::
        push_back_impl_tmpl(const V& value, typename boost::enable_if<boost::is_same<V, T>, void*>::type /* = 0 */)
        {
            return m_values.push_back(value);
        }

    template <typename T>
        template <typename V>
        void data_table_column<T>::
        push_back_impl_tmpl(const V& value, typename boost::disable_if<boost::is_same<V, T>, void*>::type /* = 0 */)
        {
            boost::throw_exception(variant_error("Column types do not match in call to push_back()"));
        }

    // const_iterator begin() const
    template <typename T>
        template <typename V>
        typename std::vector<V>::const_iterator data_table_column<T>::
        begin_impl_tmpl(typename boost::enable_if<boost::is_same<V, T>, void*>::type /* = 0 */) const
        {
            return m_values.begin();
        }

    template <typename T>
        template <typename V>
        typename std::vector<V>::const_iterator data_table_column<T>::
        begin_impl_tmpl(typename boost::disable_if<boost::is_same<V, T>, void*>::type /* = 0 */) const
        {
            boost::throw_exception(variant_error("Column types do not match in call to const begin()"));
        }

    // const_iterator end() const
    template <typename T>
        template <typename V>
        typename std::vector<V>::const_iterator data_table_column<T>::
        end_impl_tmpl(typename boost::enable_if<boost::is_same<V, T>, void*>::type /* = 0 */) const
        {
            return m_values.end();
        }

    template <typename T>
        template <typename V>
        typename std::vector<V>::const_iterator data_table_column<T>::
        end_impl_tmpl(typename boost::disable_if<boost::is_same<V, T>, void*>::type /* = 0 */) const
        {
            boost::throw_exception(variant_error("Column types do not match in call to const end()"));
        }

    // iterator begin()
    template <typename T>
        template <typename V>
        typename std::vector<V>::iterator data_table_column<T>::
        begin_impl_tmpl(typename boost::enable_if<boost::is_same<V, T>, void*>::type /* = 0 */)
        {
            return m_values.begin();
        }

    template <typename T>
        template <typename V>
        typename std::vector<V>::iterator data_table_column<T>::
        begin_impl_tmpl(typename boost::disable_if<boost::is_same<V, T>, void*>::type /* = 0 */)
        {
            boost::throw_exception(variant_error("Column types do not match in call to begin()"));
        }

    // iterator end()
    template <typename T>
        template <typename V>
        typename std::vector<V>::iterator data_table_column<T>::
        end_impl_tmpl(typename boost::enable_if<boost::is_same<V, T>, void*>::type /* = 0 */)
        {
            return m_values.end();
        }

    template <typename T>
        template <typename V>
        typename std::vector<V>::iterator data_table_column<T>::
        end_impl_tmpl(typename boost::disable_if<boost::is_same<V, T>, void*>::type /* = 0 */)
        {
            boost::throw_exception(variant_error("Column types do not match in call to end()"));
        }

}} // namespace protean::detail
