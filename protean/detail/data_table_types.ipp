//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

namespace protean { namespace detail {

    template <typename Begin, typename End, typename TypePredicateFunctor, typename TypeFunctor>
    boost::optional<typename TypeFunctor::result_type>
    call_if<Begin, End, TypePredicateFunctor, TypeFunctor>::call(const TypePredicateFunctor& pred, TypeFunctor& method)
    {
        if (pred(static_cast<typename Begin::type*>(0)))
            return boost::optional<typename TypeFunctor::result_type>(method(static_cast<typename Begin::type*>(0)));

        return call_if<typename boost::mpl::next<Begin>::type, End, TypePredicateFunctor, TypeFunctor>::call(pred, method);
    }

    template <typename End, typename TypePredicateFunctor, typename TypeFunctor>
    boost::optional<typename TypeFunctor::result_type>
    call_if<End, End, TypePredicateFunctor, TypeFunctor>::call(const TypePredicateFunctor&, TypeFunctor&)
    {
        return boost::optional<typename TypeFunctor::result_type>();
    }


    template <typename TypeFunctor>
    typename TypeFunctor::result_type data_table_runtime_type_map(
        variant_base::enum_type_t enum_type,
        TypeFunctor& method,
        const std::string& error_message)
    {
        if ((enum_type & variant::Primitive) == 0)
            boost::throw_exception(variant_error(boost::str(
                boost::format(error_message) % variant_base::enum_to_string(enum_type)
            )));

        typedef typename boost::mpl::begin<data_table_type_pairs>::type begin;
        typedef typename boost::mpl::end<data_table_type_pairs>::type   end;

        boost::optional<typename TypeFunctor::result_type> result =
            call_if<begin, end, data_table_runtime_type_comparer, TypeFunctor>::call(
                data_table_runtime_type_comparer(enum_type),
                method
            );

        if (!result)
            boost::throw_exception(variant_error("Case exhaustion: " + variant_base::enum_to_string(enum_type)));

        return result.get();
    }

}} // namespace protean::detail
