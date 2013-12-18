//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DETAIL_DATA_TABLE_TYPES_HPP
#define PROTEAN_DETAIL_DATA_TABLE_TYPES_HPP

#include <protean/config.hpp>
#include <protean/variant_base.hpp>

#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/transform.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/mpl/filter_view.hpp>
#include <boost/mpl/not_equal_to.hpp>
#include <boost/mpl/lambda.hpp>

#include <boost/mpl/fold.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/not_equal_to.hpp>
#include <boost/mpl/comparison.hpp>
#include <boost/mpl/bitand.hpp>
#include <boost/mpl/bitwise.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/optional.hpp>
#include <boost/throw_exception.hpp>

#if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable:4251)
#endif

namespace protean { namespace detail {

    /* Supported column types. *Have to manually keep up-to-date*, as they cannot be defined programmatically */
    /**********************************************************************************************************/
    #define COLUMN_TYPES                                                \
        ((variant_base::Boolean,    bool))                              \
        ((variant_base::DateTime,   boost::posix_time::ptime))          \
        ((variant_base::Time,       boost::posix_time::time_duration))  \
        ((variant_base::Date,       boost::gregorian::date))            \
        ((variant_base::Double,     double))                            \
        ((variant_base::Float,      float))                             \
        ((variant_base::UInt64,     boost::uint64_t))                   \
        ((variant_base::Int64,      boost::int64_t))                    \
        ((variant_base::UInt32,     boost::uint32_t))                   \
        ((variant_base::Int32,      boost::int32_t))                    \
        ((variant_base::Any,        detail::string))                    \
        ((variant_base::String,     detail::string))                    \
        ((variant_base::Sequence,   variant))                           \
        ((variant_base::List,       variant))                           \
        ((variant_base::Tuple,      variant))                           \
        ((variant_base::Mapping,    variant))                           \
        ((variant_base::Dictionary, variant))                           \
        ((variant_base::Bag,        variant))                           \
        ((variant_base::Collection, variant))                           \
        ((variant_base::TimeSeries, variant))

    /* Selects first and second element from tuple pair, respectively */
    /******************************************************************/
    #define GET_ENUM(pair)  BOOST_PP_TUPLE_ELEM(2, 0, pair)
    #define GET_TYPE(pair)  BOOST_PP_TUPLE_ELEM(2, 1, pair)

    /* Supported primitive types: effectively the image of the COLUMN_TYPES mapping minus the variant type */
    /*******************************************************************************************************/
    #define SUPPORTED_TYPES                 \
        (bool)                              \
        (boost::posix_time::ptime)          \
        (boost::posix_time::time_duration)  \
        (boost::gregorian::date)            \
        (double)                            \
        (float)                             \
        (boost::uint64_t)                   \
        (boost::int64_t)                    \
        (boost::uint32_t)                   \
        (boost::int32_t)                    \
        (detail::string)

    /* Compile-time mapping from variant_base::enum_type_t to corresponding type; undefined if no such pair. */
    /*********************************************************************************************************/
    template <variant_base::enum_type_t E>
    struct data_table_type_map;

    template <>
    struct data_table_type_map<variant_base::None>
    {
        typedef boost::tuples::null_type type;
    };

    #define COLUMN_TYPE_MAP_SPECIALIZATION(r, data, elem)  \
        template <>                                        \
        struct data_table_type_map<GET_ENUM(elem)>         \
        {                                                  \
            typedef GET_TYPE(elem) type;                   \
        };

    BOOST_PP_SEQ_FOR_EACH(COLUMN_TYPE_MAP_SPECIALIZATION, _, COLUMN_TYPES)

    /* MPL vector of the mpl::int-wrapped column enums */
    /***************************************************/
    #define MAKE_MPL_ENUM(s, data, elem) boost::mpl::int_<GET_ENUM(elem)>

    typedef boost::mpl::vector<
        BOOST_PP_SEQ_ENUM( BOOST_PP_SEQ_TRANSFORM(MAKE_MPL_ENUM, , COLUMN_TYPES) )
    > data_table_column_enums;

    /* Filtering of data_table_column_enums type vector to include only primitive enum types */
    /*****************************************************************************************/
    struct data_table_column_primitive_enums :
        boost::mpl::fold<
            data_table_column_enums,
            boost::mpl::vector<>,
            boost::mpl::if_<
                boost::mpl::not_equal_to<
                    boost::mpl::bitand_<boost::mpl::int_<variant_base::Primitive>, boost::mpl::_2>,
                    boost::mpl::int_<0>
                >,
                boost::mpl::push_back<boost::mpl::_1, boost::mpl::_2>,
                boost::mpl::_1
            >
        >::type
    {};


    /* Maximum number of data table columns (currently limited by maximum size of Boost.Tuple */
    /******************************************************************************************/
    #define DATA_TABLE_MAX_COLUMNS 50

    /* Prefix used for template type/enum parameters; e.g. if T, then T0, T1, ... T9 */
    /*********************************************************************************/
    #define DATA_TABLE_COLUMN_TYPE_PREFIX T


    /* Runtime mapper that takes a type-wrapped enum value and calls the given functor's operator() */
    /* with the assocaited type as a *template* parameter.  Replaces the need to write large and    */
    /* repetitive switch statements that need to be manually updated when the enum values change:   */
    /* switch(enum_value) { case A: method<A>(args); break; ... lots more cases ... }               */
    /*   <=>                                                                                        */
    /* enum_runtime_map(enum_value, method, "Error message on case exhaustion")                     */
    template <
        typename Sequence,
        template <class> class TypePredicate,
        typename EnumType,
        typename TypeFunctor
    >
    typename TypeFunctor::result_type
    enum_runtime_map(EnumType enum_value, TypeFunctor& method, const std::string& error_message);

    // Compile-time enum equality comparer
    template <typename Enum>
    struct enum_equality_comparer
    {
        enum_equality_comparer(Enum value)
            : m_value(value)
        {}

        template <typename TypedEnum>
        bool operator()(TypedEnum* = 0) const
        {
            return TypedEnum::value == m_value;
        }

    private:
        const Enum m_value;
    };

}} // namespace protean::detail

#include <protean/detail/data_table_types.ipp>


#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#endif // PROTEAN_DETAIL_DATA_TABLE_TYPES_HPP
