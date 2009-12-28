//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_OBJECT_HPP
#define PROTEAN_OBJECT_HPP

#include <protean/config.hpp>

#include <protean/detail/ref_counted.hpp>

#include <string>

namespace protean {

    class variant;
    template<typename T> class handle;

    class PROTEAN_DLLEXPORT object : public detail::ref_counted
    {
    public:
        object();
        object(const object& rhs);
        virtual ~object();

        template<typename TYPE>
        static object create();

        virtual std::string name() const = 0;

    public:
        void coerce(const object& rhs);

        int compare(const object& rhs) const;
        size_t hash() const;

    public:
        virtual int version() const = 0;
        virtual void deflate(variant& params) const = 0;
        virtual void inflate(const variant& params, int version) = 0;
        virtual handle<object> clone() const = 0;
    };

} // namespace protean

#endif // PROTEAN_OBJECT_HPP
