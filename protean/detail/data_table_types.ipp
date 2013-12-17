//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

namespace protean { namespace detail {

    namespace
    {
        /* Implementation helper for enum_runtime_map: traverses an MPL sequence until `pred' is true,  */
        /* at which point executes `method' and returns early with its result. If `pred' is never true, */
        /* returns an empty boost::optional value                                                       */
        /************************************************************************************************/
        template <typename Begin, typename End, typename TypePredicateFunctor, typename TypeFunctor>
        struct new_call_if
        {
            static boost::optional<typename TypeFunctor::result_type>
            apply(const TypePredicateFunctor& pred, TypeFunctor& method)
            {
                if (pred(static_cast<typename Begin::type*>(0)))
                    return boost::optional<typename TypeFunctor::result_type>(method(static_cast<typename Begin::type*>(0)));

                return new_call_if<typename boost::mpl::next<Begin>::type, End, TypePredicateFunctor, TypeFunctor>::apply(pred, method);
            }
        };

        template <typename End, typename TypePredicateFunctor, typename TypeFunctor>
        struct new_call_if<End, End, TypePredicateFunctor, TypeFunctor>
        {
            static boost::optional<typename TypeFunctor::result_type>
            apply(const TypePredicateFunctor&, TypeFunctor&)
            {
                return boost::optional<typename TypeFunctor::result_type>();
            }
        };
    }

    template <
        typename Sequence,
        template <class> class TypePredicate,
        typename EnumType,
        typename TypeFunctor
    >
    typename TypeFunctor::result_type
    enum_runtime_map(EnumType enum_value, TypeFunctor& method, const std::string& error_message)
    {
        typedef typename boost::mpl::begin<Sequence>::type begin;
        typedef typename boost::mpl::end<Sequence>::type   end;

        boost::optional<typename TypeFunctor::result_type> result =
            new_call_if<begin, end, TypePredicate<EnumType>, TypeFunctor>::apply(
                TypePredicate<EnumType>(enum_value),
                method
            );

        if (!result)
            boost::throw_exception(variant_error(error_message));

        return result.get();
    }

}} // namespace protean::detail
