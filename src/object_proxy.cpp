//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/variant.hpp>
#include <protean/object_proxy.hpp>

namespace protean {

    object_proxy::object_proxy(const std::string& class_name) :
        m_class_name(class_name),
        m_version(0),
        m_params(variant::Dictionary)
    {
    }
    std::string object_proxy::name() const
    {
        return m_class_name;
    }
    int object_proxy::version() const
    {
        return m_version;
    }
    void object_proxy::deflate(variant& params) const
    {
        params = m_params;
    }
    void object_proxy::inflate(const variant& params, int version)
    {
        m_version = version;
        m_params = params;
    }
    handle<object> object_proxy::clone() const
    {            
        return new object_proxy(*this);
    }        


} // namespace protean
