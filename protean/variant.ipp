//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
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
    variant::variant(T value, typename boost::enable_if<boost::is_arithmetic<T> >::type*)
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
    template<> PROTEAN_DECL bool variant::lexical_cast<bool>(const std::string& arg);
    template<> PROTEAN_DECL float variant::lexical_cast<float>(const std::string& arg);
    template<> PROTEAN_DECL double variant::lexical_cast<double>(const std::string& arg);
    template<> PROTEAN_DECL variant::date_t variant::lexical_cast<variant::date_t>(const std::string& arg);
    template<> PROTEAN_DECL variant::time_t variant::lexical_cast<variant::time_t>(const std::string& arg);
    template<> PROTEAN_DECL variant::date_time_t variant::lexical_cast<variant::date_time_t>(const std::string& arg);

    // SOURCE -> std::string
    template<> PROTEAN_DECL std::string variant::lexical_cast<std::string>(const bool& arg);
    template<> PROTEAN_DECL std::string variant::lexical_cast<std::string>(const float& arg);
    template<> PROTEAN_DECL std::string variant::lexical_cast<std::string>(const double& arg);
    template<> PROTEAN_DECL std::string variant::lexical_cast<std::string>(const variant::date_t& arg);
    template<> PROTEAN_DECL std::string variant::lexical_cast<std::string>(const variant::time_t& arg);
    template<> PROTEAN_DECL std::string variant::lexical_cast<std::string>(const variant::date_time_t& arg);

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
    template<> bool PROTEAN_DECL variant::is<std::string>()                    const;
    template<> bool PROTEAN_DECL variant::is<bool>()                           const;
    template<> bool PROTEAN_DECL variant::is<variant::date_t>()                const;
    template<> bool PROTEAN_DECL variant::is<variant::time_t>()                const;
    template<> bool PROTEAN_DECL variant::is<variant::date_time_t>()           const;
    template<> bool PROTEAN_DECL variant::is<void*>()                          const;

    template<typename T>
    typename boost::enable_if<boost::is_base_of<object,T>, bool>::type
    variant::is() const
    {
        if (!is<Object>()) return false; 
        handle<object> o = m_value.get<Object>(); 
        if (o.is<T>()) return true;
        if (o.is<object_proxy>())
        {
            return o->name()==T().name();
        }
        return false;
    }

    template<> bool PROTEAN_DECL variant::is<object>()                          const;
    template<> bool PROTEAN_DECL variant::is<object_proxy>()                    const;
    template<> bool PROTEAN_DECL variant::is<exception_data>()                  const;
    template<> bool PROTEAN_DECL variant::is<typed_array>()                     const;

    /*
     * variant::as()
     */
    template<typename T>
    typename boost::enable_if<variant::return_value<T>, T>::type
    variant::as() const
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Any | type_to_enum<T>::value, "as<" + typeid(T).name() + ">()")

        if (is<Any>())
        {
            return lexical_cast<T>(m_value.get<Any>().value());
        }
        else
        {
            return m_value.get<type_to_enum<T>::value>();
        }

        END_TRANSLATE_ERROR();
    }

    template<> PROTEAN_DECL std::string                 variant::as<std::string>()              const;
    template<> PROTEAN_DECL bool                        variant::as<bool>()                     const;
    template<> PROTEAN_DECL variant::date_t             variant::as<variant::date_t>()          const;
    template<> PROTEAN_DECL variant::time_t             variant::as<variant::time_t>()          const;
    template<> PROTEAN_DECL variant::date_time_t        variant::as<variant::date_time_t>()     const;

    template<typename T>
    typename boost::enable_if<variant::return_pointer<T>, const T>::type
    variant::as() const
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Buffer, "as<" + typeid(T).name() + ">()");

        const handle<detail::buffer>& obj(m_value.get<Buffer>());

        return reinterpret_cast<const T>(obj->data());

        END_TRANSLATE_ERROR();
    }

    template<typename T>
    typename boost::enable_if<variant::return_reference<T>, const T&>::type
    variant::as() const
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Object, "as<" + typeid(T).name() + ">()")

        const handle<object>& obj(m_value.get<Object>());

        if (obj.is<T>())
        {
            return *obj.as<T>();
        }
        else if (obj.is<object_proxy>())
        {
            // Coerce the object proxy into T
            T* casted = new T();
            casted->coerce(*obj.as<object_proxy>());

            const_cast<handle<object>&>(obj) = casted;
            return *casted;
        }
        else
        {
            boost::throw_exception(variant_error(std::string("Attempt to coerce ") + typeid(*obj).name() + " into object of type " + typeid(T).name()));
        }

        END_TRANSLATE_ERROR();
    }
    
    template<> PROTEAN_DECL const exception_data&       variant::as<exception_data>()           const;
    template<> PROTEAN_DECL const typed_array&          variant::as<typed_array>()              const;
    template<> PROTEAN_DECL const object&               variant::as<object>()                   const;

    template<typename T>
    T variant::numerical_cast() const
    {
        BEGIN_TRANSLATE_ERROR();

        CHECK_VARIANT_FUNCTION(Number, "numerical_cast<T>()");

        switch (m_type)
        {
        case Boolean:
            return static_cast<T>(m_value.get<Boolean>());
        case Int32:
            return static_cast<T>(m_value.get<Int32>());
        case UInt32:
            return static_cast<T>(m_value.get<UInt32>());
        case Int64:
            return static_cast<T>(m_value.get<Int64>());
        case UInt64:
            return static_cast<T>(m_value.get<UInt64>());
        case Float:
            return static_cast<T>(m_value.get<Float>());
        case Double:
            return static_cast<T>(m_value.get<Double>());
        default:
            boost::throw_exception(variant_error("Case exhaustion: " + enum_to_string(m_type)));
        }

        END_TRANSLATE_ERROR();
    }

    template<typename T>
    variant make_object(const variant& params)
    {
        handle<object> obj(new T());
        obj->inflate(params, obj->version());
        return variant(obj);
    }


} // namespace protean

#include <protean/detail/variant_macros_undef.hpp>
