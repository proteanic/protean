//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_OBJECT_HPP
#define PROTEAN_OBJECT_HPP

#include <protean/config.hpp>

#include <string>

namespace protean {

    class variant;
    class object_handle;

    class PROTEAN_DLLEXPORT object
    {
    public:
        object();
        object(const object& rhs);
        virtual ~object();

        template<typename TYPE>
        static object create();

        virtual std::string name() const;

    public:
        void coerce(const object& rhs);

        int compare(const object& rhs) const;
        size_t hash() const;

    public:
        virtual int version() const = 0;
        virtual void deflate(variant& params) const = 0;
        virtual void inflate(const variant& params, int version) = 0;
        virtual object_handle clone() const = 0;

    private:
        size_t    m_ref_count;
        friend class object_handle;
    };

} // namespace protean

#endif // PROTEAN_OBJECT_HPP