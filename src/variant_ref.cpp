#include <protean/variant_ref.hpp>
#include <protean/variant.hpp>

namespace protean {

    variant_ref::variant_ref(variant_base* value, variant_base::enum_type_t type) :
        m_value(value),
        m_type(type)
    {
    }

    variant_ref::variant_ref() :
        m_value(nullptr),
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

    variant_ref::operator bool() const
    {
        return m_value!=nullptr;
    }


    variant_cref::variant_cref(const variant_base* value, variant_base::enum_type_t type) :
        m_value(value),
        m_type(type)
    {
    }

    variant_cref::variant_cref() :
        m_value(nullptr),
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
        return m_value!=nullptr;
    }

} // namespace protean
