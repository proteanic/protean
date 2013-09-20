//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DETAIL_DATA_TABLE_TYPES_HPP
#define PROTEAN_DETAIL_DATA_TABLE_TYPES_HPP

#include <protean/config.hpp>
#include <protean/variant_base.hpp>

#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/seq/transform.hpp>
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

    /* Pairs that associate each supported variant_base::enum_type_t with its corresponding type */
    /*********************************************************************************************/
    struct data_table_type_pairs :
        boost::mpl::fold<
            variant_base::variant_impl_t::mpl_types_info_by_enum,
            boost::mpl::map<>,
            boost::mpl::if_<
                boost::mpl::not_equal_to<
                    boost::mpl::bitand_<
                        boost::mpl::first<boost::mpl::_2>,
                        boost::mpl::int_<variant_base::Primitive>
                    >,
                    boost::mpl::int_<0>
                >,
                boost::mpl::insert<
                    boost::mpl::_1,
                    boost::mpl::pair<
                        boost::mpl::first<boost::mpl::_2>,
                        boost::mpl::front<boost::mpl::second<boost::mpl::_2> >
                    >
                >,
                boost::mpl::_1
            >
        >::type
    {};

    /* Supported column types -- a projection of the second column of data_table_type_pairs; that is, all */
    /* primitive variant types. Have to manually keep up-to-date as cannot be defined programmatically.   */
    /******************************************************************************************************/
    #define PROTEAN_DETAIL_DATA_TABLE_COLUMN_SUPPORTED_TYPES \
        (bool)                                               \
        (boost::posix_time::ptime)                           \
        (boost::posix_time::time_duration)                   \
        (boost::gregorian::date)                             \
        (double)                                             \
        (float)                                              \
        (boost::uint64_t)                                    \
        (boost::int64_t)                                     \
        (boost::uint32_t)                                    \
        (boost::int32_t)                                     \
        (detail::string)

    /* Maximum number of data table columns (currently limited by maximum size of Boost.Tuple */
    /******************************************************************************************/
    #define DATA_TABLE_MAX_COLUMNS 10

    /* Prefix used for template type/enum parameters; e.g. if T, then T0, T1, ... T9 */
    /*********************************************************************************/
    #define DATA_TABLE_COLUMN_TYPE_PREFIX T

    /* Compile-time mapping from variant_base::enum_type_t to corresponding type; undefined if no such pair */
    /********************************************************************************************************/
    template <variant_base::enum_type_t T>
    struct data_table_type_map
        : public boost::enable_if_c<
            static_cast<bool>(T & variant_base::Primitive),
            typename boost::mpl::at<data_table_type_pairs, boost::mpl::int_<T> >::type
          >
    {};

    template <>
    struct data_table_type_map<variant_base::None>
    {
        typedef boost::tuples::null_type type;
    };

    /* Runtime mapper that takes a variant_base::enum_type_t value and calls the given functor's    */
    /* operator() with the associated type as a *template* parameter.  Replaces need to write large */
    /* repetitive switch statements that need to be manually updated:                               */
    /* switch (enum_type) { case DateTime: method<DateTime>(args); break; ... lots more cases ... } */
    /*   <=>                                                                                        */
    /* data_table_runtime_type_map(enum_type, method_functor(args));                                */
    /************************************************************************************************/
    template <typename TypeFunctor>
    typename TypeFunctor::result_type data_table_runtime_type_map(
        variant_base::enum_type_t enum_type,
        TypeFunctor& method,
        const std::string& error_message = "Cannot use non-primitive type '%s' with DataTable");

    /* Implementation helper for runtime mapper: traverses an MPL sequence, executing `method' */
    /* and returning early if `pred' is true for an element                                    */
    template <typename Begin, typename End, typename TypePredicateFunctor, typename TypeFunctor>
    struct call_if
    {
        static boost::optional<typename TypeFunctor::result_type>
        call(const TypePredicateFunctor& pred, TypeFunctor& method);
    };

    template <typename End, typename TypePredicateFunctor, typename TypeFunctor>
    struct call_if<End, End, TypePredicateFunctor, TypeFunctor>
    {
        static boost::optional<typename TypeFunctor::result_type>
        call(const TypePredicateFunctor&, TypeFunctor&);
    };

    // Default enum comparer (returns true iff enum_type_t matches)
    struct data_table_runtime_type_comparer
    {
        data_table_runtime_type_comparer(variant_base::enum_type_t type)
            : m_type(type)
        {}

        template <typename DataTableTypePair>
        bool operator()(DataTableTypePair* = 0) const
        {
            return DataTableTypePair::first::value == m_type;
        }

    private:
        const variant_base::enum_type_t m_type;
    };

}} // namespace protean::detail

#include <protean/detail/data_table_types.ipp>


#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#endif // PROTEAN_DETAIL_DATA_TABLE_TYPES_HPP
