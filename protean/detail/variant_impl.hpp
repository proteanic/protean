#ifndef PROTEAN_DETAIL_VARIANT_IMPL_HPP
#define PROTEAN_DETAIL_VARIANT_IMPL_HPP

#include <protean/config.hpp>

#include <boost/blank.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/map.hpp>

#include <boost/mpl/pair.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/string.hpp>

#include <boost/array.hpp>
#include <boost/tuple/tuple.hpp>

#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_params_with_a_default.hpp>

#if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable:4251)
#endif

#ifndef VARIANT_MAX_ARITY
    #define VARIANT_MAX_ARITY 30
#endif

#define VARIANT_VEC_TYPE BOOST_PP_CAT(vector,VARIANT_MAX_ARITY)
#define VARIANT_VEC_HEADER BOOST_PP_CAT(vector,VARIANT_MAX_ARITY).hpp
#include BOOST_PP_STRINGIZE(boost/mpl/vector/VARIANT_VEC_HEADER)

#define VARIANT_MAP_HEADER BOOST_PP_CAT(map,VARIANT_MAX_ARITY).hpp
#include BOOST_PP_STRINGIZE(boost/mpl/map/VARIANT_MAP_HEADER)

namespace protean { namespace detail {

    struct alloc_storage
    {
        union
        {
            boost::uint64_t        m_val;
            void*                m_ptr;
        };
    };

    // placement-new allocator
    struct alloc_placement
    {
        template <typename TYPE>
        static void set(const TYPE& value, alloc_storage& v)
        {
            BOOST_STATIC_ASSERT(sizeof(TYPE)<=sizeof(v.m_val));
            new (&v.m_val) TYPE(value);
        }

        template <typename TYPE>
        static void destroy(alloc_storage& v)
        {
            BOOST_STATIC_ASSERT(sizeof(TYPE)<=sizeof(v.m_val));
            reinterpret_cast<TYPE*>(&v.m_val)->~TYPE();
        }

        template <typename TYPE>
        static TYPE& get(alloc_storage& v)
        {
            BOOST_STATIC_ASSERT(sizeof(TYPE)<=sizeof(v.m_val));
            return *reinterpret_cast<TYPE*>(&v.m_val);
        }
        template <typename TYPE>
        static const TYPE& get(const alloc_storage& v)
        {
            BOOST_STATIC_ASSERT(sizeof(TYPE)<=sizeof(v.m_val));
            return *reinterpret_cast<const TYPE*>(&v.m_val);
        }
    };

    // heap allocator
    struct alloc_new
    {
        template <typename TYPE>
        static void set(const TYPE& value, alloc_storage& v)
        {
            v.m_ptr=new TYPE(value);
        }

        template <typename TYPE>
        static void destroy(alloc_storage& v)
        {
            delete reinterpret_cast<TYPE*>(v.m_ptr);
        }

        template <typename TYPE>
        static TYPE& get(alloc_storage& v)
        {
            return *reinterpret_cast<TYPE*>(v.m_ptr);
        }

        template <typename TYPE>
        static const TYPE& get(const alloc_storage& v)
        {
            return *reinterpret_cast<const TYPE*>(v.m_ptr);
        }
    };

    template<int TAG, typename TYPE, typename ALLOC_TRAIT, typename NAME>
    struct tagged_type : public boost::mpl::pair<boost::mpl::int_<TAG>, boost::mpl::vector<TYPE, ALLOC_TRAIT, NAME> >
    {};

    template<BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(VARIANT_MAX_ARITY, class T, boost::blank)>
    class PROTEAN_DECL variant_impl : protected alloc_storage
    {
        typedef boost::mpl::VARIANT_VEC_TYPE<BOOST_PP_ENUM_PARAMS(VARIANT_MAX_ARITY, T)> mpl_types;
        typedef variant_impl<BOOST_PP_ENUM_PARAMS(VARIANT_MAX_ARITY, T)>     self;

        struct mpl_types_info_by_enum : 
            boost::mpl::fold<
                mpl_types, 
                boost::mpl::map<>,
                boost::mpl::if_<
                    boost::is_same<
                        boost::mpl::_2,
                        boost::blank
                    >,
                    boost::mpl::_1,
                    boost::mpl::insert<
                        boost::mpl::_1,
                        boost::mpl::_2
                    >
                >
            >::type
        {};

        enum {
            TYPE_COUNT = boost::mpl::size<mpl_types_info_by_enum>::value
        };

        template <int N>
        struct getType :
            boost::mpl::at<
                typename boost::mpl::at<mpl_types_info_by_enum, boost::mpl::int_<N> >::type,
                boost::mpl::int_<0>
            >
        {};

        template <int N>
        struct getStorage :
            boost::mpl::at<
                typename boost::mpl::at<mpl_types_info_by_enum, boost::mpl::int_<N> >::type,
                boost::mpl::int_<1>
            >
        {};

        template <int N>
        struct getName :
            boost::mpl::at<
                typename boost::mpl::at<mpl_types_info_by_enum, boost::mpl::int_<N> >::type,
                boost::mpl::int_<2>
            >
        {};

        // conversion between enum and string
        typedef std::string (*string_void_t)(void); 

        typedef boost::tuple<
            string_void_t
        > fn_tuple_t;
        
        typedef boost::array<fn_tuple_t, TYPE_COUNT>    fn_array_t;
        typedef std::map<std::string, int>                str_to_enum_map_t;

        static fn_array_t                                sm_runtime_fn_dispatch;
        static str_to_enum_map_t                        sm_str_to_enum_map;

        template<int N>
        static std::string getName_impl() 
        {
            return boost::mpl::c_str<typename getName<N>::type>::value;
        }

        struct enumToIndex
        {
            static size_t convert(const int enumValue)
            {
                return enumToIndex_impl<TYPE_COUNT, typename boost::mpl::begin<mpl_types_info_by_enum>::type>::value(enumValue);
            }

#if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable:4702)
#endif
            template<int N, typename ITER>
            struct enumToIndex_impl
            {
                static size_t value(const int enumValue)
                {
                    if (boost::mpl::first<typename boost::mpl::deref<ITER>::type>::type::value == enumValue)
                        return TYPE_COUNT-N;
                    else
                        return enumToIndex_impl<N-1, typename boost::mpl::next<ITER>::type>::value(enumValue);
                }
            };
#if defined(_MSC_VER)
#    pragma warning(pop)
#endif
            template<typename ITER>
            struct enumToIndex_impl<0, ITER>
            {
                static size_t value(const int enumValue)
                {
                    boost::throw_exception(variant_error((boost::format("Enum value %d is out of range") % enumValue).str()));
                }
            };
        };

        struct type_at_functor
        {
            template<typename T>
            void operator()(T&)
            {
                sm_runtime_fn_dispatch[enumToIndex::convert(boost::mpl::first<T>::type::value)].template get<0>()=
                    &self::template getName_impl<boost::mpl::first<T>::type::value>
                ;

                sm_str_to_enum_map[getName_impl<boost::mpl::first<T>::type::value>()]=boost::mpl::first<T>::type::value;
            }
        };

        /* static code runner - not used as it would make variant dependent */
        struct staticDataInitialiser
        {
            staticDataInitialiser()
            {
                boost::mpl::for_each<mpl_types_info_by_enum>(type_at_functor());
            }
        };
        struct staticDataInitialiser_runner
        {
            staticDataInitialiser_runner()
            {
                static staticDataInitialiser inst;
            }
        };

    public:

        // Construct variant corresponding to enum from value
        template<int N>
        void set(const typename getType<N>::type& value) 
        {
            getStorage<N>::type::set(value, *this);
        }

        // Dispose variant corresponding to enum from value
        template<int N>
        void destroy()
        {
            getStorage<N>::type::template destroy<typename getType<N>::type>(*this);
        }

        // Get variant corresponding to enum
        template<int N>
        typename getType<N>::type& get()
        {
            return getStorage<N>::type::template get<typename getType<N>::type>(*this);
        }

        // Get variant corresponding to enum
        template<int N>
        const typename getType<N>::type& get() const
        {
            // g++ will fail if this is done on one line
            typedef typename getStorage<N>::type storage;
            typedef typename getType<N>::type thetype;
            return storage::template get<thetype>(*this);
        }

        // Swap contents of self and rhs
        void swap(variant_impl &rhs)
        {
            std::swap(m_val, rhs.m_val);
        }

        // Get type string from enum
        static std::string enum_to_string(int n)
        {
            static staticDataInitialiser_runner makeSureDataIsInitialised;
            return (sm_runtime_fn_dispatch[enumToIndex::convert(n)].template get<0>())();
        }

        // Get enum from type string
        static int string_to_enum(const std::string& type)
        {
            static staticDataInitialiser_runner makeSureDataIsInitialised;
            str_to_enum_map_t::const_iterator citr(sm_str_to_enum_map.find(type));
            if (citr==sm_str_to_enum_map.end())
            {
                boost::throw_exception(variant_error(std::string("Type string '") + type + "' not found"));
            }
            return citr->second;
        }
    };

    template<BOOST_PP_ENUM_PARAMS(VARIANT_MAX_ARITY, class T)>
    typename variant_impl<BOOST_PP_ENUM_PARAMS(VARIANT_MAX_ARITY, T)>::fn_array_t
        variant_impl<BOOST_PP_ENUM_PARAMS(VARIANT_MAX_ARITY, T)>::sm_runtime_fn_dispatch;

    template<BOOST_PP_ENUM_PARAMS(VARIANT_MAX_ARITY, class T)>
    typename variant_impl<BOOST_PP_ENUM_PARAMS(VARIANT_MAX_ARITY, T)>::str_to_enum_map_t
        variant_impl<BOOST_PP_ENUM_PARAMS(VARIANT_MAX_ARITY, T)>::sm_str_to_enum_map;

}} // namespace protean::detail

#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#endif // PROTEAN_DETAIL_VARIANT_IMPL_HPP
