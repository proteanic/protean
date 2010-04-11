//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_VARIANT_REF_IPP
#define PROTEAN_VARIANT_REF_IPP

namespace protean
{
    template<typename T>
    const variant_ref& variant_ref::operator=(const T& rhs) const
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

    template<typename T>
    T variant_cref::as() const
    {
        return variant(*this).as<T>();
    }

} // protean

#endif
