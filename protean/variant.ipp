//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/object_proxy.hpp>

namespace protean {

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
    static TARGET variant::lexical_cast(const SOURCE& arg)
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
        // If you get here, then T is unsupported
        BOOST_STATIC_ASSERT( false );
    }
    template<> bool PROTEAN_DLLEXPORT variant::is<std::string>()                    const;
    template<> bool PROTEAN_DLLEXPORT variant::is<bool>()                           const;
    template<> bool PROTEAN_DLLEXPORT variant::is<int>()                            const;
    template<> bool PROTEAN_DLLEXPORT variant::is<unsigned int>()                   const;
    template<> bool PROTEAN_DLLEXPORT variant::is<variant::int32_t>()               const;
    template<> bool PROTEAN_DLLEXPORT variant::is<variant::uint32_t>()              const;
    template<> bool PROTEAN_DLLEXPORT variant::is<variant::int64_t>()               const;
    template<> bool PROTEAN_DLLEXPORT variant::is<variant::uint64_t>()              const;
    template<> bool PROTEAN_DLLEXPORT variant::is<float>()                          const;
    template<> bool PROTEAN_DLLEXPORT variant::is<double>()                         const;
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
        return obj->name()==object::create<T>()->name();
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
        // If you get here, then T is unsupported
        BOOST_STATIC_ASSERT( false );
    }

    template<> PROTEAN_DLLEXPORT std::string                    variant::as<std::string>()            const;
    template<> PROTEAN_DLLEXPORT bool                           variant::as<bool>()                    const;
    template<> PROTEAN_DLLEXPORT int                            variant::as<int>()                    const;
    template<> PROTEAN_DLLEXPORT unsigned int                   variant::as<unsigned int>()            const;
    template<> PROTEAN_DLLEXPORT variant::int32_t               variant::as<variant::int32_t>()        const;
    template<> PROTEAN_DLLEXPORT variant::uint32_t              variant::as<variant::uint32_t>()    const;
    template<> PROTEAN_DLLEXPORT variant::int64_t               variant::as<variant::int64_t>()        const;
    template<> PROTEAN_DLLEXPORT variant::uint64_t              variant::as<variant::uint64_t>()    const;
    template<> PROTEAN_DLLEXPORT float                          variant::as<float>()                const;
    template<> PROTEAN_DLLEXPORT double                         variant::as<double>()                const;
    template<> PROTEAN_DLLEXPORT variant::date_t                variant::as<variant::date_t>()        const;
    template<> PROTEAN_DLLEXPORT variant::time_t                variant::as<variant::time_t>()        const;
    template<> PROTEAN_DLLEXPORT variant::date_time_t           variant::as<variant::date_time_t>()    const;
    template<> PROTEAN_DLLEXPORT void*                          variant::as<void*>()                const;
    template<> PROTEAN_DLLEXPORT exception_info                 variant::as<exception_info>()        const;

    template<typename T>
    typename boost::enable_if<boost::is_base_of<object, T>, const T&>::type
    variant::as() const
    {
        if (m_type!=Object)
        {
            boost::throw_exception(variant_error(std::string("Attempt to cast ") + enum_to_string(type()) + " to Object"));
        }

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

            const_cast<object&>(obj) = casted;
            return *casted;
        }
        else
        {
            boost::throw_exception(variant_error(std::string("Attempt to coerce ") + typeid(*obj).name() + " into object of type " + typeid(T).name()));
        }
    }

    template<> PROTEAN_DLLEXPORT const object& variant::as<object>() const;

    template<typename T>
    typename boost::enable_if<boost::is_base_of<object, T>, T&>::type
    variant::as()
    {
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
    }

    template<> PROTEAN_DLLEXPORT object& variant::as<object>();

} // namespace protean
