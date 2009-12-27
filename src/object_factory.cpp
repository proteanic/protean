//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/object_factory.hpp>
#include <protean/handle.hpp>

namespace protean {

    /*static*/ object_factory::instance_map_t object_factory::sm_instance_map;

    /*static*/ void object_factory::insert (const std::string & class_name, handle<object> (*fn) ())
    {
        create_fn_t create_fn(fn);
        std::pair<instance_map_t::iterator, bool> ret =
            sm_instance_map.insert( std::make_pair(class_name, create_fn) );

        if (!ret.second)
        {
            boost::throw_exception(variant_error("Duplicate item '" + class_name + "' detected in object factory"));
        }
    }

    /*static*/ void object_factory::initialise()
    {
    }

    /*static*/ void object_factory::shutdown()
    {
        sm_instance_map.clear();
    }

    /*static*/ handle<object> object_factory::create_instance(const std::string& name)
    {
        instance_map_t::const_iterator citr(sm_instance_map.find(name));

        if (citr == sm_instance_map.end())
        {
            // Return empty handle
            return handle<object>(NULL);
        }

        return citr->second();
    }

} // namespace protean
