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
        template <variant_base::enum_type_t E>
        class data_table_column;
    }

    /* boost::tuples::tuple is limited to 10 elements.  The row typedef takes up to DATA_TABLE_MAX_COLUMNS     */
    /* types and type arguments and constructs the underlying boost::tuples::cons type sequence.  That is,     */
    /* row<T0, ..., TN>::type is type-equivalent to tuple<T0, ..., TN> if N were allowed to be large for       */
    /* boost::tuples::tuple. They are compatible with boost::tuples::tuple.                                    */
    /***********************************************************************************************************/
    #define ENUM_PREFIX E
    #define TYPE_PREFIX T
    #define ELEMENT_0(prefix) BOOST_PP_CAT(prefix, 0)
    #define IDENTITY(z, n, text) text

    template <
        typename HT,
        BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(
            BOOST_PP_DEC(DATA_TABLE_MAX_COLUMNS),
            typename TYPE_PREFIX,
            boost::tuples::null_type
        )
    >
    struct row
    {
        typedef boost::tuples::cons<HT,
            typename row<
                BOOST_PP_ENUM_PARAMS(BOOST_PP_DEC(DATA_TABLE_MAX_COLUMNS), TYPE_PREFIX),
                boost::tuples::null_type
            >::type
        > type;
    };

    template <>
    struct row<BOOST_PP_ENUM(DATA_TABLE_MAX_COLUMNS, IDENTITY, boost::tuples::null_type)>
    {
        typedef boost::tuples::null_type type;
    };

    /* Overloaded helper functions for constructing large tuples (rows) */
    /********************************************************************/
    template <typename T0>
    inline typename row<T0>::type make_row(const T0& t0)
    {
        return typename row<T0>::type(t0);
    }

    #define COLUMN_PARAMETER(z, n, text) const text ##n & t##n

    #define MAKE_ROW_OVERLOAD(z, n, text)                                   \
    template <                                                              \
        BOOST_PP_ENUM_PARAMS(n, typename TYPE_PREFIX)                       \
    >                                                                       \
    typename row<BOOST_PP_ENUM_PARAMS(n, TYPE_PREFIX)>::type make_row(      \
        BOOST_PP_ENUM(n, COLUMN_PARAMETER, TYPE_PREFIX)                     \
    )                                                                       \
    {                                                                       \
        return typename row<BOOST_PP_ENUM_PARAMS(n, TYPE_PREFIX)>::type(    \
            t0,                                                             \
            make_row(BOOST_PP_ENUM_SHIFTED_PARAMS(n, t))                    \
        );                                                                  \
    }

    BOOST_PP_REPEAT_FROM_TO(2, BOOST_PP_INC(DATA_TABLE_MAX_COLUMNS), MAKE_ROW_OVERLOAD, _)


    /* data_table_row is a typedef for creating rows using the variant enum type value */
    /***********************************************************************************/
    #define DATA_TABLE_ENUM_TO_TYPE(z, n, data) \
        typename detail::data_table_type_map<data ## n>::type

    template <
        BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(
            DATA_TABLE_MAX_COLUMNS,
            variant_base::enum_type_t ENUM_PREFIX,
            variant_base::None
        )
    >
    struct data_table_row
    {
        typedef typename row<
            BOOST_PP_ENUM(DATA_TABLE_MAX_COLUMNS, DATA_TABLE_ENUM_TO_TYPE, ENUM_PREFIX)
        >::type type;
    };


    /* Non-variant (typed) iterators ***********************************************/
    /* Essentially a boost::zip_iterator of the underlying column vector iterators */
    /*******************************************************************************/

    // Typed const DataTable iterator
    #define DATA_TABLE_COLUMN_CONST_ITERATOR_TYPE(z, n, e) \
        typename detail::data_table_column<e ## n>::const_iterator

    template <
        BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(
            DATA_TABLE_MAX_COLUMNS,
            variant_base::enum_type_t ENUM_PREFIX,
            variant_base::None
        )
    >
    class data_table_const_iterator
        : public boost::zip_iterator<
            typename row<
                BOOST_PP_ENUM(
                    DATA_TABLE_MAX_COLUMNS,
                    DATA_TABLE_COLUMN_CONST_ITERATOR_TYPE,
                    ENUM_PREFIX
                )
            >::type
        >
    {
    public:
        typedef typename row<
            BOOST_PP_ENUM(
                DATA_TABLE_MAX_COLUMNS,
                DATA_TABLE_COLUMN_CONST_ITERATOR_TYPE,
                ENUM_PREFIX
            )
        >::type iterator_tuple_type;

        typedef boost::zip_iterator<iterator_tuple_type> super;

        typedef typename data_table_row<
            BOOST_PP_ENUM_PARAMS(DATA_TABLE_MAX_COLUMNS, ENUM_PREFIX)
        >::type value_type;

    public:
        data_table_const_iterator()
        {}

        data_table_const_iterator(iterator_tuple_type t)
            : super(t)
        {}

        data_table_const_iterator(const data_table_const_iterator& other)
            : super(other.get_iterator_tuple())
        {}

        const typename detail::data_table_type_map<ELEMENT_0(ENUM_PREFIX)>::type& time() const
        {
            return boost::get<0>(*(*this));
        }

        template <size_t N>
        typename boost::tuples::access_traits<
            typename boost::tuples::element<N, value_type>::type
        >::const_type get() const
        {
            return boost::get<N>(*(*this));
        }
    };


    // Typed non-const DataTable iterator
    #define DATA_TABLE_COLUMN_ITERATOR_TYPE(z, n, e) \
        typename detail::data_table_column<e ## n>::iterator

    template <
        BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(
            DATA_TABLE_MAX_COLUMNS,
            variant_base::enum_type_t ENUM_PREFIX,
            variant_base::None
        )
    >
    class data_table_iterator
        : public boost::zip_iterator<
            typename row<
                BOOST_PP_ENUM(
                    DATA_TABLE_MAX_COLUMNS,
                    DATA_TABLE_COLUMN_ITERATOR_TYPE,
                    ENUM_PREFIX
                )
            >::type
        >
    {
    public:
        typedef typename row<
            BOOST_PP_ENUM(
                DATA_TABLE_MAX_COLUMNS,
                DATA_TABLE_COLUMN_ITERATOR_TYPE,
                ENUM_PREFIX
            )
        >::type iterator_tuple_type;

        typedef boost::zip_iterator<iterator_tuple_type> super;

        typedef typename data_table_row<
            BOOST_PP_ENUM_PARAMS(DATA_TABLE_MAX_COLUMNS, ENUM_PREFIX)
        >::type value_type;

    public:
        data_table_iterator()
        {}

        data_table_iterator(iterator_tuple_type t)
            : super(t)
        {}

        data_table_iterator(const data_table_iterator& other)
            : super(other.get_iterator_tuple())
        {}

        typename detail::data_table_type_map<ELEMENT_0(ENUM_PREFIX)>::type& time()
        {
            return boost::get<0>(*(*this));
        }

        template <size_t N>
        typename boost::tuples::access_traits<
            typename boost::tuples::element<N, value_type>::type
        >::non_const_type get()
        {
            return boost::get<N>(*(*this));
        }

        // Implicit conversion to non-const DataTable iterator
        operator data_table_const_iterator<
            BOOST_PP_ENUM_PARAMS(DATA_TABLE_MAX_COLUMNS, ENUM_PREFIX)
        >()
        {
            return data_table_const_iterator<
                BOOST_PP_ENUM_PARAMS(DATA_TABLE_MAX_COLUMNS, ENUM_PREFIX)
            >(this->get_iterator_tuple());
        }
    };


    /* STL container-like helper typedefs */
    /**************************************/
    template <
        BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(
            DATA_TABLE_MAX_COLUMNS,
            variant_base::enum_type_t ENUM_PREFIX,
            variant_base::None
        )
    >
    struct data_table
    {
        typedef typename data_table_row<
            BOOST_PP_ENUM_PARAMS(DATA_TABLE_MAX_COLUMNS, ENUM_PREFIX)
        >::type value_type;

        typedef data_table_iterator<
            BOOST_PP_ENUM_PARAMS(DATA_TABLE_MAX_COLUMNS, ENUM_PREFIX)
        > iterator;

        typedef data_table_const_iterator<
            BOOST_PP_ENUM_PARAMS(DATA_TABLE_MAX_COLUMNS, ENUM_PREFIX)
        > const_iterator;
    };

} // namespace protean

#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#endif // PROTEAN_DATA_TABLE_ITERATOR_HPP
