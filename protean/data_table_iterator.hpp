//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DATA_TABLE_ITERATOR_HPP
#define PROTEAN_DATA_TABLE_ITERATOR_HPP

#include <protean/config.hpp>
#include <protean/variant_base.hpp>
#include <protean/detail/data_table_types.hpp>

#include <boost/preprocessor/repetition/enum_params_with_a_default.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/utility.hpp>

#if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable:4251)
#endif

namespace protean {

    // Forward declaration
    namespace detail {
        template <typename T>
        class data_table_column;
    }

    /* Non-variant (typed) iterators ***********************************************/
    /* Essentially a boost::zip_iterator of the underlying column vector iterators */
    /*******************************************************************************/

    // Tuple of actual column types, parameterised by corresponding enum type values
    #define DATA_TABLE_TUPLE_TYPE(z, n, data) \
        typename detail::data_table_type_map<data ## n>::type

    template <
        BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(
            DATA_TABLE_MAX_COLUMNS,
            variant_base::enum_type_t DATA_TABLE_COLUMN_TYPE_PREFIX,
            variant_base::None
        )
    >
    struct data_table_tuple
    {
        typedef boost::tuple<
            BOOST_PP_ENUM(DATA_TABLE_MAX_COLUMNS, DATA_TABLE_TUPLE_TYPE, DATA_TABLE_COLUMN_TYPE_PREFIX)
        > type;
    };


    // Typed const DataTable iterator
    #define	DATA_TABLE_COLUMN_CONST_ITERATOR_TYPE(z, n, data) \
        typename detail::data_table_column<DATA_TABLE_TUPLE_TYPE(z, n, data)>::const_iterator

    template <
        BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(
            DATA_TABLE_MAX_COLUMNS,
            variant_base::enum_type_t DATA_TABLE_COLUMN_TYPE_PREFIX,
            variant_base::None
        )
    >
    class data_table_const_iterator : public boost::zip_iterator<
        boost::tuple<
            BOOST_PP_ENUM(DATA_TABLE_MAX_COLUMNS, DATA_TABLE_COLUMN_CONST_ITERATOR_TYPE, DATA_TABLE_COLUMN_TYPE_PREFIX)
        > >
    {
    public:
        typedef boost::tuple<
            BOOST_PP_ENUM(DATA_TABLE_MAX_COLUMNS, DATA_TABLE_COLUMN_CONST_ITERATOR_TYPE, DATA_TABLE_COLUMN_TYPE_PREFIX)
        > iterator_tuple_type;

        typedef boost::zip_iterator<iterator_tuple_type> super;

    public:
        data_table_const_iterator()
        {}

        data_table_const_iterator(iterator_tuple_type t)
            : super(t)
        {}

        data_table_const_iterator(const data_table_const_iterator& other)
            : super(other.get_iterator_tuple())
        {}

        const typename detail::data_table_type_map<T0>::type& time() const
        {
            return boost::get<0>(*(*this));
        }

        template <size_t N>
        typename boost::tuples::access_traits<
            typename boost::tuples::element<
                N,
                typename data_table_tuple<
                    BOOST_PP_ENUM_PARAMS(DATA_TABLE_MAX_COLUMNS, DATA_TABLE_COLUMN_TYPE_PREFIX)
                >::type
            >::type
        >::const_type get() const
        {
            return boost::get<N>(*(*this));
        }
    };


    // Typed non-const DataTable iterator
    #define	DATA_TABLE_COLUMN_ITERATOR_TYPE(z, n, data) \
        typename detail::data_table_column<DATA_TABLE_TUPLE_TYPE(z, n, data)>::iterator

    template <
        BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(
            DATA_TABLE_MAX_COLUMNS,
            variant_base::enum_type_t DATA_TABLE_COLUMN_TYPE_PREFIX,
            variant_base::None
        )
    >
    class data_table_iterator : public boost::zip_iterator<
        boost::tuple<
            BOOST_PP_ENUM(DATA_TABLE_MAX_COLUMNS, DATA_TABLE_COLUMN_ITERATOR_TYPE, DATA_TABLE_COLUMN_TYPE_PREFIX)
        > >
    {
    public:
        typedef boost::tuple<
            BOOST_PP_ENUM(DATA_TABLE_MAX_COLUMNS, DATA_TABLE_COLUMN_ITERATOR_TYPE, DATA_TABLE_COLUMN_TYPE_PREFIX)
        > iterator_tuple_type;

        typedef boost::zip_iterator<iterator_tuple_type> super;

    public:
        data_table_iterator()
        {}

        data_table_iterator(iterator_tuple_type t)
            : super(t)
        {}

        data_table_iterator(const data_table_iterator& other)
            : super(other.get_iterator_tuple())
        {}

        const typename detail::data_table_type_map<T0>::type& time() const
        {
            return boost::get<0>(*(*this));
        }

        template <size_t N>
        typename boost::tuples::access_traits<
            typename boost::tuples::element<
                N,
                typename data_table_tuple<
                    BOOST_PP_ENUM_PARAMS(DATA_TABLE_MAX_COLUMNS, DATA_TABLE_COLUMN_TYPE_PREFIX)
                >::type
            >::type
        >::non_const_type get()
        {
            return boost::get<N>(*(*this));
        }

        // Implicit conversion to non-const DataTable iterator
        operator data_table_const_iterator<
            BOOST_PP_ENUM_PARAMS(DATA_TABLE_MAX_COLUMNS, DATA_TABLE_COLUMN_TYPE_PREFIX)
        >()
        {
            return data_table_const_iterator<
                BOOST_PP_ENUM_PARAMS(DATA_TABLE_MAX_COLUMNS, DATA_TABLE_COLUMN_TYPE_PREFIX)
            >(get_iterator_tuple());
        }
    };


    /* STL container-like helper typedefs */
    /**************************************/
    template <
        BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(
            DATA_TABLE_MAX_COLUMNS,
            variant_base::enum_type_t DATA_TABLE_COLUMN_TYPE_PREFIX,
            variant_base::None
        )
    >
    struct data_table
    {
        typedef typename data_table_tuple<
            BOOST_PP_ENUM_PARAMS(DATA_TABLE_MAX_COLUMNS, DATA_TABLE_COLUMN_TYPE_PREFIX)
        >::type value_type;

        typedef data_table_iterator<
            BOOST_PP_ENUM_PARAMS(DATA_TABLE_MAX_COLUMNS, DATA_TABLE_COLUMN_TYPE_PREFIX)
        > iterator;

        typedef data_table_const_iterator<
            BOOST_PP_ENUM_PARAMS(DATA_TABLE_MAX_COLUMNS, DATA_TABLE_COLUMN_TYPE_PREFIX)
        > const_iterator;
    };

} // namespace protean

#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#endif // PROTEAN_DATA_TABLE_ITERATOR_HPP
