//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/detail/buffer.hpp>

#include <boost/functional/hash.hpp>

namespace protean { namespace detail {

    buffer::buffer() :
        m_size(0),
        m_data(NULL)
    {
    }

    buffer::buffer(void* data, size_t size, bool copyData) :
        m_size(size)
    {
        if (copyData)
        {
            m_data = malloc( size );
            memcpy( m_data, data, size );
        }
        else
        {
            m_data = data;
        }
    }

    buffer::buffer(const buffer& rhs) :
        m_size(rhs.size())
    {
        m_data = malloc(m_size);
        memcpy(m_data, rhs.data(), m_size);
    }

    buffer::~buffer()
    {
        if (m_data)
        {
            free(m_data);
            m_data = NULL;
            m_size = 0;
        }
    }

    void* buffer::data() const
    {
        return m_data;
    }

    size_t buffer::size() const
    {
        return m_size;
    }

    int buffer::compare(const buffer& rhs) const
    {
        if (m_size==rhs.m_size)
        {
            return std::memcmp(m_data, rhs.m_data, m_size);
        }
        else
        {
            return (m_size < rhs.m_size ? -1 : 1);
        }
    }

    size_t buffer::hash() const
    {
        const char* char_array( static_cast<const char*>(m_data) );
        return boost::hash_range(char_array, char_array+m_size);
    }

}} // namespace protean::detail
