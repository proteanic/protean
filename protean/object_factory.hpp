//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_OBJECT_FACTORY_HPP
#define PROTEAN_OBJECT_FACTORY_HPP

#include <protean/config.hpp>
#include <protean/object.hpp>
#include <protean/handle.hpp>
#include <protean/variant.hpp>
#include <protean/object_proxy.hpp>

#include <boost/function.hpp>

#include <string>
#include <map>

#if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable:4251)
#endif

namespace protean {

    template<typename T> class handle;

    class PROTEAN_DECL object_factory
    {
    public:
        typedef boost::function<handle<object>()> create_fn_t;
        typedef std::map<std::string, create_fn_t> instance_map_t;

    public:
        void initialise();
        void shutdown();

        template<class T>
        typename boost::enable_if<boost::is_base_of<object,T>, void>::type 
        register_object();

        handle<object> create_instance(const std::string& name);

    private:
        void insert(const std::string& class_name, const create_fn_t& fn);
        instance_map_t m_instance_map;
    };

    template<> PROTEAN_DECL
    void object_factory::register_object<object_proxy>();

    template<class T>
    typename boost::enable_if<boost::is_base_of<object,T>, void>::type 
    object_factory::register_object()
    {
        insert(T().name(), &handle<object>::create<T>);
    }
} // namespace protean

#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#endif // PROTEAN_OBJECT_FACTORY_HPP
