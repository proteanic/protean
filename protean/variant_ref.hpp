//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_VARIANT_REF_HPP
#define PROTEAN_VARIANT_REF_HPP

#include <protean/config.hpp>
#include <protean/variant_base.hpp>

namespace protean {

    class variant;
    class variant_cref;
    class typed_array;
    
    // mutable reference
    class PROTEAN_DECL variant_ref
    {
    private:
        variant_ref(variant_base* value, variant_base::enum_type_t type);

    public:
        variant_ref();
        explicit variant_ref(variant& arg);
        
        variant_ref(const variant_ref& rhs);
        variant_ref& operator=(const variant_ref& rhs);

        template<typename T>
        variant_ref& operator=(const T& rhs);

        template<typename T>
        T as() const;

        operator bool() const;

    private:
        friend class variant;
        friend class variant_cref;
        friend class typed_array;

        variant_base*               m_value;
        variant_base::enum_type_t   m_type;
    };

    template<typename T>
    variant_ref& variant_ref::operator=(const T& rhs)
    {
        variant tmp(rhs);

        if (tmp.type()!=m_type)
        {
            boost::throw_exception(variant_error((boost::format("Attempt to assign variant of type %s to reference of type %s")
                % variant::enum_to_string(tmp.type())
                % variant::enum_to_string(m_type)
            ).str()));
        }

        m_value->swap(tmp);

        return *this;
    }

    template<typename T>
    T variant_ref::as() const
    {
        return variant(*this).as<T>();
    }

    // const reference
    class PROTEAN_DECL variant_cref
    {
    private:
        variant_cref(const variant_base* value, variant_base::enum_type_t type);

    public:
        variant_cref();
        explicit variant_cref(const variant& arg);
        
        variant_cref(const variant_cref& rhs);
        variant_cref& operator=(const variant_cref& rhs);

        // non-const to const conversion
        variant_cref(const variant_ref& rhs);
        variant_cref& operator=(const variant_ref& rhs);

        template<typename T>
        T as() const;

        operator bool() const;

    private:
        friend class variant;
        friend class typed_array;

        const variant_base*         m_value;
        variant_base::enum_type_t   m_type;
    };


    template<typename T>
    T variant_cref::as() const
    {
        return variant(*this).as<T>();
    }

} // namespace protean

#endif // PROTEAN_VARIANT_REF_HPP
