//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <boost/preprocessor/seq/for_each.hpp>

namespace protean { namespace detail {

    /************************/
    /* data_table_column<E> */
    /************************/

    template <variant_base::enum_type_t E>
    data_table_column<E>::data_table_column(const std::string& name, size_t capacity /* = 0 */)
        : data_table_column_base(name, E)
    {
        if (capacity > 0)
            m_values.reserve(capacity);
    }

    template <variant_base::enum_type_t E>
    data_table_column_base* data_table_column<E>::clone() const
    {
        return new data_table_column(*this);
    }

    template <variant_base::enum_type_t E>
    void data_table_column<E>::resize(size_t n)
    {
        m_values.resize(n);
    }

    template <variant_base::enum_type_t E>
    bool data_table_column<E>::empty() const
    {
        return m_values.empty();
    }

    template <variant_base::enum_type_t E>
    size_t data_table_column<E>::size() const
    {
        return m_values.size();
    }

    template <variant_base::enum_type_t E>
    void data_table_column<E>::clear()
    {
        m_values.clear();
    }

    template <variant_base::enum_type_t E>
    variant_const_iterator_base* data_table_column<E>::begin() const
    {
        return new data_table_column_variant_iterator_interface<typename data_table_column<E>::value_type>(m_values.begin());
    }

    template <variant_base::enum_type_t E>
    variant_const_iterator_base* data_table_column<E>::end() const
    {
        return new data_table_column_variant_iterator_interface<typename data_table_column<E>::value_type>(m_values.end());
    }

    template <variant_base::enum_type_t E>
    variant_iterator_base* data_table_column<E>::begin()
    {
        return new data_table_column_variant_iterator_interface<typename data_table_column<E>::value_type, iterator_traits>(m_values.begin());
    }

    template <variant_base::enum_type_t E>
    variant_iterator_base* data_table_column<E>::end()
    {
        return new data_table_column_variant_iterator_interface<typename data_table_column<E>::value_type, iterator_traits>(m_values.end());
    }

    /* Typed method implementation template-dispatchers */
    /****************************************************/
    #define PROTEAN_DETAIL_DT_COLUMN_VIRTUAL_IMPLS_IMPLS(r, data, elem)                                                                            \
        template <variant_base::enum_type_t E>                                                                                                     \
        std::vector<GET_TYPE(elem)>::const_iterator data_table_column<E>::begin_impl(column_traits<GET_ENUM(elem)>::enum_type* p /* = 0 */) const  \
        { return begin_impl_tmpl<GET_ENUM(elem)>(p); }                                                                                             \
                                                                                                                                                   \
        template <variant_base::enum_type_t E>                                                                                                     \
        std::vector<GET_TYPE(elem)>::const_iterator data_table_column<E>::end_impl(column_traits<GET_ENUM(elem)>::enum_type* p /* = 0 */) const    \
        { return end_impl_tmpl<GET_ENUM(elem)>(p); }                                                                                               \
                                                                                                                                                   \
        template <variant_base::enum_type_t E>                                                                                                     \
        std::vector<GET_TYPE(elem)>::iterator data_table_column<E>::begin_impl(column_traits<GET_ENUM(elem)>::enum_type* p /* = 0 */)              \
        { return begin_impl_tmpl<GET_ENUM(elem)>(p); }                                                                                             \
                                                                                                                                                   \
        template <variant_base::enum_type_t E>                                                                                                     \
        std::vector<GET_TYPE(elem)>::iterator data_table_column<E>::end_impl(column_traits<GET_ENUM(elem)>::enum_type* p /* = 0 */)                \
        { return end_impl_tmpl<GET_ENUM(elem)>(p); }

    #define PROTEAN_DETAIL_DT_COLUMN_VIRTUAL_PUSH_BACK_IMPL_DEF(r, data, type)     \
        template <variant_base::enum_type_t E>                                     \
        void data_table_column<E>::push_back_impl(const type& value, type* p /* = 0 */)  \
        { push_back_impl_tmpl<type>(value, p); }

    template <variant_base::enum_type_t E>
    void data_table_column<E>::push_back_impl(const variant& value, variant* p = 0)
    { push_back_variant_impl_tmpl<variant>(value, p); }

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable:4702)
#endif

    BOOST_PP_SEQ_FOR_EACH(PROTEAN_DETAIL_DT_COLUMN_VIRTUAL_PUSH_BACK_IMPL_DEF, , SUPPORTED_TYPES)
    BOOST_PP_SEQ_FOR_EACH(PROTEAN_DETAIL_DT_COLUMN_VIRTUAL_IMPLS_IMPLS, , COLUMN_TYPES)
    
#if defined(_MSC_VER)
#   pragma warning(pop)
#endif

    /* Typed method template implementations */
    /*****************************************/

    // void push_back()
    template <variant_base::enum_type_t E>
        template <typename V>
        void data_table_column<E>::
        push_back_impl_tmpl(const V& value, typename boost::enable_if<boost::is_same<V, typename data_table_column<E>::value_type>, void*>::type /* = 0 */)
        {
            return m_values.push_back(value);
        }

    template <variant_base::enum_type_t E>
        template <typename V>
        void data_table_column<E>::
        push_back_impl_tmpl(const V& /*value*/, typename boost::disable_if<boost::is_same<V, typename data_table_column<E>::value_type>, void*>::type /* = 0 */)
        {
            boost::throw_exception(variant_error("Column types do not match in call to push_back()"));
        }

    // void push_back(variant)
    template <variant_base::enum_type_t E>
        template <typename V>
        void data_table_column<E>::push_back_variant_impl_tmpl(const variant& value, typename boost::enable_if<is_variant<value_type, V>, void*>::type /* = 0 */)
        {
            if ((value.type() > E) || (value.type() & E) == 0)
                boost::throw_exception(variant_error("Column variant types do not match in call to variant push_back()"));

            m_values.push_back(value);
        }

    template <variant_base::enum_type_t E>
        template <typename V>
        void data_table_column<E>::push_back_variant_impl_tmpl(const variant& /*value*/, typename boost::disable_if<is_variant<value_type, V>, void*>::type /* = 0 */)
        {
            boost::throw_exception(variant_error("Column types do not match in call to variant push_back()"));
        }

    // const_iterator begin() const
    template <variant_base::enum_type_t E>
        template <variant_base::enum_type_t F>
        typename column_traits<F>::const_iterator
        data_table_column<E>::begin_impl_tmpl(typename boost::enable_if<is_same_column<E, F>, void*>::type /* = 0 */) const
        {
            return m_values.begin();
        }

    template <variant_base::enum_type_t E>
        template <variant_base::enum_type_t F>
        typename column_traits<F>::const_iterator
        data_table_column<E>::begin_impl_tmpl(typename boost::disable_if<is_same_column<E, F>, void*>::type /* = 0 */) const
        {
            boost::throw_exception(variant_error("Column types do not match in call to const begin()"));
        }

    // const_iterator end() const
    template <variant_base::enum_type_t E>
        template <variant_base::enum_type_t F>
        typename column_traits<F>::const_iterator
        data_table_column<E>::end_impl_tmpl(typename boost::enable_if<is_same_column<E, F>, void*>::type /* = 0 */) const
        {
            return m_values.end();
        }

    template <variant_base::enum_type_t E>
        template <variant_base::enum_type_t F>
        typename column_traits<F>::const_iterator
        data_table_column<E>::end_impl_tmpl(typename boost::disable_if<is_same_column<E, F>, void*>::type /* = 0 */) const
        {
            boost::throw_exception(variant_error("Column types do not match in call to const end()"));
        }

    // iterator begin()
    template <variant_base::enum_type_t E>
        template <variant_base::enum_type_t F>
        typename column_traits<F>::iterator
        data_table_column<E>::begin_impl_tmpl(typename boost::enable_if<is_same_column<E, F>, void*>::type /* = 0 */)
        {
            return m_values.begin();
        }

    template <variant_base::enum_type_t E>
        template <variant_base::enum_type_t F>
        typename column_traits<F>::iterator
        data_table_column<E>::begin_impl_tmpl(typename boost::disable_if<is_same_column<E, F>, void*>::type /* = 0 */)
        {
            boost::throw_exception(variant_error("Column types do not match in call to begin()"));
        }

    // iterator end()
    template <variant_base::enum_type_t E>
        template <variant_base::enum_type_t F>
        typename column_traits<F>::iterator
        data_table_column<E>::end_impl_tmpl(typename boost::enable_if<is_same_column<E, F>, void*>::type /* = 0 */)
        {
            return m_values.end();
        }

    template <variant_base::enum_type_t E>
        template <variant_base::enum_type_t F>
        typename column_traits<F>::iterator
        data_table_column<E>::end_impl_tmpl(typename boost::disable_if<is_same_column<E, F>, void*>::type /* = 0 */)
        {
            boost::throw_exception(variant_error("Column types do not match in call to end()"));
        }

}} // namespace protean::detail
