namespace protean {

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

    template <variant_base::enum_type_t E>
    typename column_traits<E>::const_iterator data_table_column_base::begin() const
    {
        return begin_impl( static_cast<typename column_traits<E>::enum_type*>(0) );
    }

    template <variant_base::enum_type_t E>
    typename column_traits<E>::const_iterator data_table_column_base::end() const
    {
        return end_impl(static_cast<typename column_traits<E>::enum_type*>(0));
    }

    template <variant_base::enum_type_t E>
    typename column_traits<E>::iterator data_table_column_base::begin()
    {
        return begin_impl(static_cast<typename column_traits<E>::enum_type*>(0));
    }

    template <variant_base::enum_type_t E>
    typename column_traits<E>::iterator data_table_column_base::end()
    {
        return end_impl(static_cast<typename column_traits<E>::enum_type*>(0));
    }

} // namespace protean
