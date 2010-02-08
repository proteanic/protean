//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/exception_data.hpp>

#include <boost/functional/hash.hpp>

#include <typeinfo>

namespace protean {

    exception_data::exception_data(const std::string& type, const std::string& message) :
        m_type(type),
        m_message(message)
    {}

    exception_data::exception_data(const std::exception& e) :
        m_type(format(typeid(e).name())),
        m_message(e.what())
    {}

    const std::string& exception_data::type() const
    {
        return m_type;
    }
    const std::string& exception_data::message() const
    {
        return m_message;
    }

    int exception_data::compare(const exception_data& rhs) const
    {
        int cmp(type().compare(rhs.type()));
        if (cmp!=0)
        {
            return cmp;
        }
        return message().compare(rhs.message());
    }
    size_t exception_data::hash() const
    {
        size_t seed = 0;
        boost::hash_combine(seed, m_type );
        boost::hash_combine(seed, m_message );

        return seed;
    }
    /*static*/ std::string exception_data::format(const char* class_name)
    {
        const std::string tmp(class_name);
        return tmp.substr(6);
    }


} // namespace protean
