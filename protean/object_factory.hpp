//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_OBJECT_FACTORY_HPP
#define PROTEAN_OBJECT_FACTORY_HPP

#include <protean/config.hpp>
#include <protean/variant_error.hpp>
#include <protean/object.hpp>

#include <boost/function.hpp>

#include <string>
#include <map>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4251)
#endif

namespace protean {

    class object_handle;

    class PROTEAN_DLLEXPORT object_factory
    {
    public:
        typedef boost::function<object_handle()> create_fn_t;
        typedef std::map<std::string, create_fn_t> instance_map_t;

    public:
        void initialise();
        void shutdown();

        template<typename TYPE>
        void register_instance(const std::string& name);

        object_handle create_instance(const std::string& name);

    private:
        static instance_map_t    sm_instance_map;
    };

    template<class TYPE>
    void object_factory::register_instance(const std::string& name="")
    {
        std::string class_name(name);
        if (class_name.empty())
        {
            class_name = typeid(TYPE).name();
            class_name = class_name.substr(6); // ignore the 'class ' prefix
        }
        create_fn_t create_fn(&object::create<TYPE>);
        std::pair<instance_map_t::iterator, bool> ret =
            sm_instance_map.insert( std::make_pair(class_name, create_fn) );

        if (!ret.second)
        {
            boost::throw_exception(variant_error("Duplicate item '" + class_name + "' detected in object factory"));
        }
    }
} // namespace protean

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // PROTEAN_OBJECT_FACTORY_HPP
