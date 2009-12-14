//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/object_proxy.hpp>

#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/type_traits/is_signed.hpp>

#include <boost/mpl/at.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/has_key.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/tuple/tuple.hpp>

#include <protean/detail/variant_macros_define.hpp>

namespace protean {

    namespace {

        template <typename T>
        struct type_to_enum
        {
            template<bool INTEGRAL, bool FLOAT, bool SIGNED, size_t SIZE>
            struct type_trait_key
            {};

            template<bool INTEGRAL, bool FLOAT, bool SIGNED, size_t SIZE, int ENUM_VALUE>
            struct type_trait_key_value :
                boost::mpl::pair<
                    type_trait_key<INTEGRAL, FLOAT, SIGNED, SIZE>,
                    boost::mpl::int_<ENUM_VALUE>
                >
            {};


            typedef boost::mpl::map<
                //                   integral  float  signed  size  type
                type_trait_key_value<true,     false, true,   4,    variant::Int32  >,
                type_trait_key_value<true,     false, false,  4,    variant::UInt32 >,
                type_trait_key_value<true,     false, true,   8,    variant::Int64  >,
                type_trait_key_value<true,     false, false,  8,    variant::UInt64 >,
                type_trait_key_value<false,    true,  false,  4,    variant::Float  >,
                type_trait_key_value<false,    true,  false,  8,    variant::Double >
            > type_construction_info;

            typedef type_trait_key<
                boost::is_integral<T>::value,
                boost::is_floating_point<T>::value,
                boost::is_signed<T>::value,
                sizeof(T)
            > type_info_key;

            // provide feedback if someone uses a unrecognised type
            BOOST_MPL_ASSERT_MSG(
                (boost::mpl::has_key<
                    type_construction_info,
                    type_info_key
                >::type::value),
                TYPE_NOT_SUPPORTED,
                (
                    typename boost::mpl::identity<T>::type
                )
            );

            static const variant::enum_type_t value = static_cast<variant::enum_type_t>(
                boost::mpl::at< type_construction_info, type_info_key >::type::value
            );
        };
    }

    template<typename T>
    variant::variant(T value, typename boost::enable_if<boost::is_pod<T> >::type*)
    {
        m_type = type_to_enum<T>::value;
        m_value.set<type_to_enum<T>::value>(value);
    }

    template<typename T>
    typename boost::enable_if<boost::is_pod<T>, variant&>::type
    variant::operator=(T value)
    {
        variant(value).swap(*this);
        return *this;
    }

    template<typename T>
    typename boost::disable_if<boost::is_pod<T>, variant&>::type
    variant::operator=(const T& value)
    {
        variant(value).swap(*this);
        return *this;
    }

    template<typename TARGET, typename SOURCE>
    TARGET variant::lexical_cast(const SOURCE& arg)
    { 
        try
        {
            // By default, perform a boost::lexical_cast
            return boost::lexical_cast<TARGET>(arg);
        }
        catch(boost::bad_lexical_cast&)
        {
            boost::throw_exception(variant_error(std::string("Bad lexical cast: failed to interpret ") + typeid(SOURCE).name() + " as " + typeid(TARGET).name()));
        }
    }
    // std::string -> TARGET
    template<> PROTEAN_DLLEXPORT bool variant::lexical_cast<bool>(const std::string& arg);
    template<> PROTEAN_DLLEXPORT float variant::lexical_cast<float>(const std::string& arg);
    template<> PROTEAN_DLLEXPORT double variant::lexical_cast<double>(const std::string& arg);
    template<> PROTEAN_DLLEXPORT variant::date_t variant::lexical_cast<variant::date_t>(const std::string& arg);
    template<> PROTEAN_DLLEXPORT variant::time_t variant::lexical_cast<variant::time_t>(const std::string& arg);
    template<> PROTEAN_DLLEXPORT variant::date_time_t variant::lexical_cast<variant::date_time_t>(const std::string& arg);

    // SOURCE -> std::string
    template<> PROTEAN_DLLEXPORT std::string variant::lexical_cast<std::string>(const bool& arg);
    template<> PROTEAN_DLLEXPORT std::string variant::lexical_cast<std::string>(const float& arg);
    template<> PROTEAN_DLLEXPORT std::string variant::lexical_cast<std::string>(const double& arg);
    template<> PROTEAN_DLLEXPORT std::string variant::lexical_cast<std::string>(const variant::date_t& arg);
    template<> PROTEAN_DLLEXPORT std::string variant::lexical_cast<std::string>(const variant::time_t& arg);
    template<> PROTEAN_DLLEXPORT std::string variant::lexical_cast<std::string>(const variant::date_time_t& arg);

    /*
     * variant::is
     */
    template<int N>
    bool variant::is() const
    {
        return (m_type & N)!=0;
    }

    template<typename T>
    typename boost::disable_if<boost::is_base_of<object,T>, bool>::type
    variant::is() const
    {
        return m_type==type_to_enum<T>::value; 
    }
    template<> bool PROTEAN_DLLEXPORT variant::is<std::string>()                    const;
    template<> bool PROTEAN_DLLEXPORT variant::is<bool>()                           const;
    template<> bool PROTEAN_DLLEXPORT variant::is<variant::date_t>()                const;
    template<> bool PROTEAN_DLLEXPORT variant::is<variant::time_t>()                const;
    template<> bool PROTEAN_DLLEXPORT variant::is<variant::date_time_t>()           const;
    template<> bool PROTEAN_DLLEXPORT variant::is<void*>()                          const;

    template<typename T>
    typename boost::enable_if<boost::is_base_of<object,T>, bool>::type
    variant::is() const
    {
        if (m_type!=Object)
        {
            return false;
        }
        
        object_handle obj(m_value.get<Object>());
        return obj->name()==T().name();
    }

    template<> bool PROTEAN_DLLEXPORT variant::is<object>() const;
    template<> bool PROTEAN_DLLEXPORT variant::is<object_proxy>() const;

    /*
     * variant::as()
     */
    template<typename T>
    typename boost::disable_if<boost::is_base_of<object, T>, T>::type
    variant::as() const
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Any | type_to_enum<T>::value, "as<" + typeid(T).name() + ">()")

        if (is<Any>())
        {
            return lexical_cast<T>(m_value.get<Any>().value());
        }
        else
        {
            return m_value.get<type_to_enum<T>::value>();
        }

        END_VARIANT_CONTEXT();
    }

    template<> PROTEAN_DLLEXPORT std::string                    variant::as<std::string>()            const;
    template<> PROTEAN_DLLEXPORT bool                           variant::as<bool>()                    const;
    template<> PROTEAN_DLLEXPORT variant::date_t                variant::as<variant::date_t>()        const;
    template<> PROTEAN_DLLEXPORT variant::time_t                variant::as<variant::time_t>()        const;
    template<> PROTEAN_DLLEXPORT variant::date_time_t           variant::as<variant::date_time_t>()    const;
    template<> PROTEAN_DLLEXPORT void*                          variant::as<void*>()                const;
    template<> PROTEAN_DLLEXPORT exception_info                 variant::as<exception_info>()        const;

    template<typename T>
    typename boost::enable_if<boost::is_base_of<object, T>, const T&>::type
    variant::as() const
    {
        BEGIN_VARIANT_CONTEXT();

        CHECK_VARIANT_FUNCTION(Object, "as<" + typeid(T).name() + ">()")

        const object_handle& obj(m_value.get<Object>());

        if (obj.is<T>())
        {
            return *obj.as<T>();
        }
        else if (obj.is<object_proxy>())
        {
            // Coerce the StreamableProxy into T
            T* casted = new T();
            casted->coerce(*obj.as<object_proxy>());

            const_cast<object_handle&>(obj) = casted;
            return *casted;
        }
        else
        {
            boost::throw_exception(variant_error(std::string("Attempt to coerce ") + typeid(*obj).name() + " into object of type " + typeid(T).name()));
        }

        END_VARIANT_CONTEXT();
    }

    template<> PROTEAN_DLLEXPORT const object& variant::as<object>() const;

    template<typename T>
    typename boost::enable_if<boost::is_base_of<object, T>, T&>::type
    variant::as()
    {
        BEGIN_VARIANT_CONTEXT();

        const T& obj(static_cast<const variant*>(this)->as<T>());

        object_handle& base(m_value.get<Object>());

        if (base.unique())
        {
            return const_cast<T&>(obj);
        }
        else
        {
            T* copy = new T(obj);
            base = copy;
            return *copy;
        }

        END_VARIANT_CONTEXT();
    }

    template<> PROTEAN_DLLEXPORT object& variant::as<object>();

} // namespace protean

#include <protean/detail/variant_macros_undef.hpp>
