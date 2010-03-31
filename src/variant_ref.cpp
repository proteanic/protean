//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/variant_ref.hpp>
#include <protean/variant.hpp>

namespace protean {

    variant_ref::variant_ref(variant_base* value, variant_base::enum_type_t type) :
        m_value(value),
        m_type(type)
    {
    }

    variant_ref::variant_ref() :
        m_value(NULL),
        m_type(variant::None)
    {
    }

    variant_ref::variant_ref(variant& arg) :
        m_value(&arg),
        m_type(arg.type())        
    {
    }
    
    variant_ref::variant_ref(const variant_ref& rhs) :
        m_value(rhs.m_value),
        m_type(rhs.m_type)        
    {
    }

    variant_ref& variant_ref::operator=(const variant_ref& rhs)
    {
        m_value = rhs.m_value;
        m_type = rhs.m_type;

        return *this;
    }

    variant_ref& variant_ref::operator=(const variant& rhs)
    {
        if (rhs.type()!=m_type)
        {
            boost::throw_exception(variant_error((boost::format("Attempt to assign variant of type %s to reference of type %s")
                % variant::enum_to_string(rhs.type())
                % variant::enum_to_string(m_type)
            ).str()));
        }

        m_value->destroy(m_type);
        m_value->assign(m_type, rhs);

        return *this;
    }

    variant_ref::operator bool() const
    {
        return m_value!=NULL;
    }


    variant_cref::variant_cref(const variant_base* value, variant_base::enum_type_t type) :
        m_value(value),
        m_type(type)
    {
    }

    variant_cref::variant_cref() :
        m_value(NULL),
        m_type(variant::None)
    {
    }

    variant_cref::variant_cref(const variant& arg) :
        m_value(&arg),
        m_type(arg.type())        
    {
    }
    
    variant_cref::variant_cref(const variant_cref& rhs) :
        m_value(rhs.m_value),
        m_type(rhs.m_type)        
    {
    }

    variant_cref& variant_cref::operator=(const variant_cref& rhs)
    {
        m_value = rhs.m_value;
        m_type = rhs.m_type;

        return *this;
    }

    variant_cref::variant_cref(const variant_ref& rhs) :
        m_value(rhs.m_value),
        m_type(rhs.m_type)    
    {
    }

    variant_cref& variant_cref::operator=(const variant_ref& rhs)
    {
        m_value = rhs.m_value;
        m_type = rhs.m_type;

        return *this;
    }

    variant_cref::operator bool() const
    {
        return m_value!=NULL;
    }

} // namespace protean
