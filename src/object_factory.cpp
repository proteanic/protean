//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/object_factory.hpp>
#include <protean/handle.hpp>

namespace protean {

    void object_factory::insert(const std::string& class_name, const create_fn_t& fn)
    {
        std::pair<instance_map_t::iterator, bool> ret =
            m_instance_map.insert( std::make_pair(class_name, fn) );

        if (!ret.second)
        {
            boost::throw_exception(variant_error("Duplicate item '" + class_name + "' detected in object factory"));
        }
    }

    void object_factory::initialise()
    {
    }

    void object_factory::shutdown()
    {
        m_instance_map.clear();
    }

    handle<object> object_factory::create_instance(const std::string& name)
    {
        instance_map_t::const_iterator citr(m_instance_map.find(name));

        if (citr == m_instance_map.end())
        {
            // Return empty handle
            return handle<object>(NULL);
        }

        return citr->second();
    }

    template<>
    void object_factory::register_object<object_proxy>()
    {
        boost::throw_exception (variant_error("Cannot register an object proxy"));
    }

} // namespace protean
