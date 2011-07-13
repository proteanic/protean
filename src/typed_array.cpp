//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/typed_array.hpp>
#include <protean/variant.hpp>
#include <protean/detail/hash.hpp>

#include <boost/bind.hpp>
#include <boost/cast.hpp>
#include <boost/function.hpp>

namespace protean {

    typed_array::typed_array(size_t size, variant_base::enum_type_t type) :
        m_size(boost::numeric_cast<boost::uint32_t>(size)),
        m_type(type)
    {
        m_data = new variant_base[m_size];
        for (size_t i=0; i<m_size; ++i)
        {
            m_data[i].initialise(m_type, 0);
        }
    }

    typed_array::typed_array(size_t size, const variant& init) :
        m_size(boost::numeric_cast<boost::uint32_t>(size)),
        m_type(init.type())
    {
        m_data = new variant_base[m_size];
        for (size_t i=0; i<m_size; ++i)
        {
            m_data[i].assign(m_type, init);
        }
    }

    typed_array::typed_array(const typed_array& rhs) :
        m_size(rhs.size()),
        m_type(rhs.type())
    {
        m_data = new variant_base[m_size];
        for (size_t i=0; i<m_size; ++i)
        {
            m_data[i].assign(m_type, rhs.m_data[i]);
        }
    }

    typed_array& typed_array::operator=(const typed_array& rhs)
    {
        typed_array(rhs).swap(*this);
        return *this;
    }

    typed_array::~typed_array()
    {
        for (size_t i=0; i<m_size; ++i)
        {
            m_data[i].destroy(m_type);
        }

        delete[] m_data;
        m_data = NULL;
        m_size = 0;
        m_type = variant::None;
    }

    variant_base::enum_type_t typed_array::type() const
    {
        return m_type;
    }

    size_t typed_array::size() const
    {
        return m_size;
    }

    static bool variant_base_less(variant_base::enum_type_t type, const variant_base& lhs, const variant_base& rhs)
    {
        return lhs.compare(type, rhs)<0;
    }

    int typed_array::compare(const typed_array& rhs) const
    {
        if (m_type!=rhs.m_type)
        {
            return m_type<rhs.type() ? -1 : 1;
        }

        boost::function<bool(const variant_base&, const variant_base&)> comp(boost::bind(&variant_base_less, m_type, _1, _2));

        return (std::lexicographical_compare(m_data, m_data + m_size, rhs.m_data, rhs.m_data + m_size, comp) ?
            -1 : (std::lexicographical_compare(rhs.m_data, rhs.m_data + m_size, m_data, m_data + m_size, comp) ? 1 : 0));
    }

    boost::uint64_t typed_array::hash(boost::uint64_t seed) const
    {
        seed = detail::hash_value((boost::uint32_t)m_type, seed);

        for (size_t i=0; i<m_size; ++i)
        {
            seed = m_data[i].hash(m_type, seed);
        }

        return seed;
    }

    void typed_array::swap(typed_array& rhs)
    {
        std::swap(m_size, rhs.m_size);
        std::swap(m_data, rhs.m_data);
        std::swap(m_type, rhs.m_type);
    }

    variant_ref typed_array::operator[](size_t n)
    {
        return variant_ref(&at(n), m_type);
    }

    variant_cref typed_array::operator[](size_t n) const
    {
        return variant_cref(&at(n), m_type);
    }

    typed_array::const_iterator typed_array::begin() const
    {
        return const_iterator(m_data, m_type);
    }

    typed_array::const_iterator typed_array::end() const
    {
        return const_iterator(m_data+size(), m_type);
    }

    typed_array::iterator typed_array::begin()
    {
        return iterator(m_data, m_type);
    }

    typed_array::iterator typed_array::end()
    {
        return iterator(m_data+size(), m_type);
    }

    variant_base& typed_array::at(size_t n)
    {
        if (n>=m_size)
        {
            boost::throw_exception(variant_error((boost::format("Index %u is out of range for Array of size %u") % n % m_size).str()));
        }
        return m_data[n];
    }

    const variant_base& typed_array::at(size_t n) const
    {
        if (n>=m_size)
        {
            boost::throw_exception(variant_error((boost::format("Index %u is out of range for Array of size %u") % n % m_size).str()));
        }
        return m_data[n];
    }

} // namespace protean
