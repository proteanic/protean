//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DETAIL_BUFFER_HPP
#define PROTEAN_DETAIL_BUFFER_HPP

#include <protean/config.hpp>

#include <protean/detail/ref_counted.hpp>

#include <cstddef>

namespace protean { namespace detail {
    
    class PROTEAN_DLLEXPORT buffer : public detail::ref_counted
    {
    public:
        buffer(size_t size);
        buffer(const void* data, size_t size);
        buffer(const buffer& rhs);
		buffer& operator=(const buffer& rhs);
        ~buffer();

        void*  data() const;
        size_t size() const;

        int compare(const buffer& rhs) const;
        size_t hash() const;

		void swap(buffer& rhs);

    private:
        size_t  m_size;
        void*   m_data;
    };

}} // namespace protean::detail

#endif // PROTEAN_DETAIL_BUFFER_HPP
