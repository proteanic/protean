//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_REF_COUNTED_HPP
#define PROTEAN_REF_COUNTED_HPP

#include <protean/config.hpp>

#include <string>

namespace protean {

    class object_handle;

    namespace detail {

    class PROTEAN_DLLEXPORT ref_counted
    {
    public:
        ref_counted();
        ref_counted(const ref_counted& rhs);
    private:
        size_t  m_ref_count;
        friend class object_handle;
    };

    inline ref_counted::ref_counted() :
        m_ref_count(0)
    {}

    inline ref_counted::ref_counted(const ref_counted& rhs) :
        m_ref_count(0)
    {}

}} // namespace protean::detail

#endif // PROTEAN_REF_COUNTED_HPP
