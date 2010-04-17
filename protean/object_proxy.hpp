//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_OBJECT_PROXY_HPP
#define PROTEAN_OBJECT_PROXY_HPP

#include <protean/config.hpp>

#include <protean/object.hpp>
#include <protean/handle.hpp>

namespace protean {

    class variant;

    class PROTEAN_DECL object_proxy : public object
    {
    public:
        object_proxy(const std::string& class_name);

        std::string name() const;
        int version() const;
        void deflate(variant& params) const;
        void inflate(const variant& params, int version);
        handle<object> clone() const;

    private:
        friend class variant;
        object_proxy();

        std::string    m_class_name;
        int            m_version;
        variant        m_params;
    };

} // namespace protean

#endif // PROTEAN_OBJECT_PROXY_HPP
