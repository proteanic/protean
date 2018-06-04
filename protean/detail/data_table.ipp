#include <protean/detail/data_table_column.hpp>

#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/format.hpp>

namespace protean { namespace detail {

    /* Column setup implementation */
    /*******************************/
    template <variant_base::enum_type_t E>
    data_table& data_table::add_column(const std::string& name)
    {
        if (!m_columns.empty() && !get_column(0).empty())
            boost::throw_exception(variant_error("Cannot add a column since values have been inserted"));

        if (m_columns.size() >= DATA_TABLE_MAX_COLUMNS)
            boost::throw_exception(variant_error("Reached maximum column capacity"));

        for ( const auto& column : m_columns )
            if (column.name() == name)
                boost::throw_exception(variant_error("Column name already in use"));

        m_columns.push_back(new data_table_column<E>(name, m_capacity));

        return *this;
    }

    /* Push-back implementation */
    /****************************/
    template <typename Tuple>
    data_table& data_table::push_back(const Tuple& value)
    {
        size_t tuple_length = boost::tuples::length<Tuple>::value;
        if (tuple_length != m_columns.size())
            boost::throw_exception(variant_error(boost::str(
                boost::format("Cannot insert value with %s elements into series with %s columns")
                % tuple_length
                % m_columns.size()
            )));

        push_back_impl<0>(value);
        return *this;
    }

    template <size_t N, typename HT, typename TT>
    void data_table::push_back_impl(const boost::tuples::cons<HT, TT>& tuple)
    {
        get_column(N).push_back(tuple.get_head());
        push_back_impl<N+1>(tuple.get_tail());
    }

    template <size_t N, typename TT>
    void data_table::push_back_impl(const boost::tuples::cons<std::string, TT>& tuple)
    {
        get_column(N).push_back( detail::string(tuple.get_head().c_str(), tuple.get_head().size()) );
        push_back_impl<N+1>(tuple.get_tail());
    }

    template <size_t N>
    void data_table::push_back_impl(const boost::tuples::null_type&)
    {}

    /* Typed iterators implementation */
    /**********************************/
    #define COLUMN_BEGIN_ITERATOR(z, n, t) \
        get_column(n).begin<t ## n>()

    #define COLUMN_END_ITERATOR(z, n, t) \
        get_column(n).end<t ## n>()

    #define BEGIN_ITERATOR_DEF(z, n, t)                                                                     \
        template <BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), variant_base::enum_type_t t)>                    \
        data_table_iterator<BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), t)> data_table::begin()                \
        {                                                                                                   \
            return data_table_iterator<BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), t)>(                        \
                make_row( BOOST_PP_ENUM(BOOST_PP_ADD(n, 1), COLUMN_BEGIN_ITERATOR, t) )                     \
            );                                                                                              \
        }

    #define END_ITERATOR_DEF(z, n, t)                                                                       \
        template <BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), variant_base::enum_type_t t)>                    \
        data_table_iterator<BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), t)> data_table::end()                  \
        {                                                                                                   \
            return data_table_iterator<BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), t)>(                        \
                make_row( BOOST_PP_ENUM(BOOST_PP_ADD(n, 1), COLUMN_END_ITERATOR, t) )                       \
            );                                                                                              \
        }

    #define BEGIN_CONST_ITERATOR_DEF(z, n, t)                                                               \
        template <BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), variant_base::enum_type_t t)>                    \
        data_table_const_iterator<BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), t)> data_table::begin() const    \
        {                                                                                                   \
            return data_table_const_iterator<BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), t)>(                  \
                make_row( BOOST_PP_ENUM(BOOST_PP_ADD(n, 1), COLUMN_BEGIN_ITERATOR, t) )                     \
            );                                                                                              \
        }

    #define END_CONST_ITERATOR_DEF(z, n, t)                                                                 \
        template <BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), variant_base::enum_type_t t)>                    \
        data_table_const_iterator<BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), t)> data_table::end() const      \
        {                                                                                                   \
            return data_table_const_iterator<BOOST_PP_ENUM_PARAMS(BOOST_PP_ADD(n, 1), t)>(                  \
                make_row( BOOST_PP_ENUM(BOOST_PP_ADD(n, 1), COLUMN_END_ITERATOR, t) )                       \
            );                                                                                              \
        }

    // data_table_iterator<T0, ..., Tn> begin() { ... }
    BOOST_PP_REPEAT(DATA_TABLE_MAX_COLUMNS, BEGIN_ITERATOR_DEF, DATA_TABLE_COLUMN_TYPE_PREFIX)
    // data_table_iterator<T0, ..., Tn> end() { ... }
    BOOST_PP_REPEAT(DATA_TABLE_MAX_COLUMNS, END_ITERATOR_DEF, DATA_TABLE_COLUMN_TYPE_PREFIX)
    // data_table_const_iterator<T0, ..., Tn> begin() const { ... }
    BOOST_PP_REPEAT(DATA_TABLE_MAX_COLUMNS, BEGIN_CONST_ITERATOR_DEF, DATA_TABLE_COLUMN_TYPE_PREFIX)
    // data_table_const_iterator<T0, ..., Tn> end() const { ... }
    BOOST_PP_REPEAT(DATA_TABLE_MAX_COLUMNS, END_CONST_ITERATOR_DEF, DATA_TABLE_COLUMN_TYPE_PREFIX)

}} // namespace protean::detail
