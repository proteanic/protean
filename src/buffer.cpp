//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/detail/buffer.hpp>

#include <protean/detail/hash.hpp>


#include <cstring>
#include <cstdlib>

namespace protean { namespace detail {

    buffer::buffer(size_t size) :
        m_size(size)
    {
        m_data = std::malloc(size);
        std::memset(m_data, 0, size);
    }

    buffer::buffer(const void* data, size_t size) :
        m_size(size)
    {
        m_data = std::malloc(size);
        std::memcpy(m_data, data, size);
    }

    buffer::buffer(const buffer& rhs) :
        m_size(rhs.size())
    {
        m_data = malloc(m_size);
        std::memcpy(m_data, rhs.data(), m_size);
    }

    buffer& buffer::operator=(const buffer& rhs)
    {
        buffer(rhs).swap(*this);
        return *this;
    }

    buffer::~buffer()
    {
        std::free(m_data);
        m_data = nullptr;
        m_size = 0;
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

    boost::uint64_t buffer::hash(boost::uint64_t seed) const
    {
        return hash_value((unsigned char*)m_data, m_size, seed);
    }

    void buffer::swap(buffer& rhs)
    {
        std::swap(m_size, rhs.m_size);
        std::swap(m_data, rhs.m_data);
    }

}} // namespace protean::detail
