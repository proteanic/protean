//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_VARIANT_BASE_HPP
#define PROTEAN_VARIANT_BASE_HPP

#include <protean/config.hpp>

#include <list>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>

#include <protean/variant_error.hpp>

#include <protean/detail/string.hpp>
#include <protean/exception_data.hpp>
#include <protean/object.hpp>
#include <protean/detail/buffer.hpp>
#include <protean/detail/collection.hpp>
#    include <protean/detail/sequence.hpp>
#        include <protean/detail/list.hpp>
#        include <protean/detail/tuple.hpp>
#    include <protean/detail/mapping.hpp>
#        include <protean/detail/bag.hpp>
#        include <protean/detail/dictionary.hpp>
#    include <protean/detail/timeseries.hpp>
#include <protean/handle.hpp>
#include <protean/detail/variant_impl.hpp>

namespace protean {

    class PROTEAN_DLLEXPORT variant_base
    {
    public:

        enum enum_type_t
        {
            None                    = 0x00000001,
            Any                     = 0x00000002,
            String                  = 0x00000004,
            Boolean                 = 0x00000008,
            Int32                   = 0x00000010,
            UInt32                  = 0x00000020,
            Int64                   = 0x00000040,
            UInt64                  = 0x00000080,
            Float                   = 0x00000100,
            Double                  = 0x00000200,
            Date                    = 0x00000400,
            Time                    = 0x00000800,
            DateTime                = 0x00001000,
            List                    = 0x00002000,
            Dictionary              = 0x00004000,
            Bag                     = 0x00008000,
            Buffer                  = 0x00010000,
            Tuple                   = 0x00020000,
            Exception               = 0x00040000,
            TimeSeries              = 0x00080000,
            Object                  = 0x00100000,

            // The following enums represent groupings of variants
            Integer                 = Boolean | Int32 | UInt32 | Int64 | UInt64,
            Number                  = Integer | Float | Double,
            Primitive               = Number | Date | Time | DateTime | Any | String,
            Sequence                = List | Tuple,
            Mapping                 = Dictionary | Bag,
            Collection              = Sequence | Mapping | TimeSeries
        };

        typedef detail::variant_impl<
            detail::tagged_type<None,               boost::blank,                       detail::alloc_placement,    boost::mpl::string<'None'> >,
            detail::tagged_type<Any,                detail::string,                     detail::alloc_placement,    boost::mpl::string<'Any'> >,
            detail::tagged_type<String,             detail::string,                     detail::alloc_placement,    boost::mpl::string<'Stri','ng'> >,
            detail::tagged_type<Int32,              boost::int32_t,                     detail::alloc_placement,    boost::mpl::string<'Int3','2'> >,
            detail::tagged_type<UInt32,             boost::uint32_t,                    detail::alloc_placement,    boost::mpl::string<'UInt','32'> >,
            detail::tagged_type<Int64,              boost::int64_t,                     detail::alloc_placement,    boost::mpl::string<'Int6','4'> >,
            detail::tagged_type<UInt64,             boost::uint64_t,                    detail::alloc_placement,    boost::mpl::string<'UInt','64'> >,
            detail::tagged_type<Float,              float,                              detail::alloc_placement,    boost::mpl::string<'Floa','t'> >,
            detail::tagged_type<Double,             double,                             detail::alloc_placement,    boost::mpl::string<'Doub','le'> >,
            detail::tagged_type<Date,               boost::gregorian::date,             detail::alloc_placement,    boost::mpl::string<'Date'> >,
            detail::tagged_type<Time,               boost::posix_time::time_duration,   detail::alloc_placement,    boost::mpl::string<'Time'> >,
            detail::tagged_type<DateTime,           boost::posix_time::ptime,           detail::alloc_placement,    boost::mpl::string<'Date','Time'> >,
            detail::tagged_type<Boolean,            bool,                               detail::alloc_placement,    boost::mpl::string<'Bool','ean'> >,
            detail::tagged_type<Collection,         detail::collection,                 detail::alloc_new,          boost::mpl::string<'Cont','aine','r'> >,
            detail::tagged_type<Sequence,           detail::sequence,                   detail::alloc_new,          boost::mpl::string<'Sequ','ence'> >,
            detail::tagged_type<List,               detail::list,                       detail::alloc_new,          boost::mpl::string<'List'> >,
            detail::tagged_type<Mapping,            detail::mapping,                    detail::alloc_new,          boost::mpl::string<'Mapp','ing'> >,
            detail::tagged_type<Dictionary,         detail::dictionary,                 detail::alloc_new,          boost::mpl::string<'Dict','iona','ry'> >,
            detail::tagged_type<Bag,                detail::bag,                        detail::alloc_new,          boost::mpl::string<'Bag'> >,
            detail::tagged_type<Object,             handle<object>,                     detail::alloc_placement,    boost::mpl::string<'Obje','ct'> >,
            detail::tagged_type<TimeSeries,         detail::timeseries,                 detail::alloc_new,          boost::mpl::string<'Time','Seri','es'> >,
            detail::tagged_type<Tuple,              detail::tuple,                      detail::alloc_new,          boost::mpl::string<'Tupl','e'> >,
            detail::tagged_type<Buffer,             handle<detail::buffer>,             detail::alloc_placement,    boost::mpl::string<'Buff','er'> >,
            detail::tagged_type<Exception,          exception_data,                     detail::alloc_new,          boost::mpl::string<'Exce','ptio','n'> >
        > variant_impl_t;
        
        variant_impl_t m_value;

    public:
        variant_base();
        variant_base(enum_type_t type, size_t size=0);
        variant_base(enum_type_t type, const variant_base& rhs);

        void destroy(enum_type_t type);

        void swap(variant_base& rhs);

        static std::string enum_to_string(enum_type_t type);
        static enum_type_t string_to_enum(const std::string& type);

    private:
        friend class xml_reader;
        friend class xml_writer;
        friend class sax_content_handler;
        friend class binary_reader;
        friend class binary_writer;
    };

} // namespace protean

#endif // PROTEAN_VARIANT_BASE_HPP
