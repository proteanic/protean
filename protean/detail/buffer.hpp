//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DETAIL_BUFFER_HPP
#define PROTEAN_DETAIL_BUFFER_HPP

#include <protean/config.hpp>

namespace protean { namespace detail {
    
    class PROTEAN_DLLEXPORT buffer
    {
    public:
        buffer();
        buffer(void* data, size_t size, bool copyData);
        buffer(const buffer& rhs);
        ~buffer();

        void*  data() const;
        size_t size() const;

        int compare(const buffer& rhs) const;
        size_t hash() const;
    private:
        void*  m_data;
        size_t m_size;
    };

}} // namespace protean::detail

#endif // PROTEAN_DETAIL_BUFFER_HPP
