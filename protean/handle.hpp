//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_HANDLE_HPP
#define PROTEAN_HANDLE_HPP

#include <protean/config.hpp>

namespace protean {

    template<typename T>
    class PROTEAN_DLLEXPORT handle
    {
    public:
        handle();
        handle(T* pointee);
        ~handle();

        handle(const handle& rhs);
        handle& operator=(const handle& rhs);

        T& operator*() const;
        T* operator->() const;

        template<typename U>
        bool is() const;

        template<typename U>
        U* as() const;

        bool unique() const;
        bool null() const;

        void swap(handle& rhs);

        template<typename U>
        static handle create();

    private:
        T*    m_pointee;
    };

} // namespace protean

#include <protean/handle.ipp>

#endif // PROTEAN_HANDLE_HPP
