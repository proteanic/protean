//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_OBJECT_HANDLE_HPP
#define PROTEAN_OBJECT_HANDLE_HPP

#include <protean/config.hpp>

#include <protean/object.hpp>

namespace protean {

    class PROTEAN_DLLEXPORT object_handle
    {
    public:
        object_handle();
        object_handle(object* pointee);
        ~object_handle();

        object_handle(const object_handle& rhs);
        object_handle& operator=(const object_handle& rhs);

        object& operator*() const;
        object* operator->() const;

        template<typename T>
        bool is() const;

        template<typename T>
        T* as() const;

        bool unique() const;
        operator bool() const;

        void swap(object_handle& rhs);

    private:
        object*    m_pointee;
    };

    template<typename T>
    bool object_handle::is() const
    {
        return as<T>()!=NULL;
    }

    template<typename T>
    T* object_handle::as() const
    {
        return dynamic_cast<T*>(m_pointee);
    }
} // namespace protean

#endif // PROTEAN_OBJECT_HANDLE_HPP
