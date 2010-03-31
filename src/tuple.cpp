//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/detail/tuple.hpp>
#include <protean/detail/tuple_iterator.hpp>
#include <protean/variant.hpp>
#include <protean/variant_error.hpp>

#include <boost/functional/hash.hpp>

namespace protean { namespace detail {

    tuple::tuple(size_t size) :
        m_size(static_cast<boost::uint16_t>(size))
    {
        if (size>0xFFFF)
        {
            boost::throw_exception(variant_error((boost::format("Size %u exceeds tuple capacity") % size).str()));
        }

        m_value = new variant[size];
    }

    tuple::tuple(const tuple& rhs) :
        m_size(rhs.m_size)
    {
        m_value = new variant[m_size];

        variant* dst = m_value;
        const variant* src = rhs.m_value;

        while(dst!=m_value+m_size)
        {
            *dst = *src;
            ++dst;
            ++src;
        }
    }

	tuple& tuple::operator=(const tuple& rhs)
	{
		tuple(rhs).swap(*this);
		return *this;
	}

    tuple::~tuple()
    {
       delete[] m_value;
    }

    int tuple::compare(const collection& rhs) const
    {
        const tuple* rhs_cast = dynamic_cast<const tuple*>(&rhs);
        if (rhs_cast==NULL)
        {
            boost::throw_exception(variant_error("Unable to cast collection to tuple"));
        }
        return (std::lexicographical_compare(m_value, m_value + m_size, rhs_cast->m_value, rhs_cast->m_value + rhs_cast->m_size) ?
            -1 : (std::lexicographical_compare(rhs_cast->m_value, rhs_cast->m_value + rhs_cast->m_size, m_value, m_value + m_size) ? 1 : 0));
    }

    size_t tuple::hash() const
    {
        return boost::hash_range(m_value, m_value + m_size);
    }

    variant& tuple::at(size_t index) 
    {
        if (index>size())
        {
            boost::throw_exception(variant_error((boost::format("Index %u is out of range for tuple of size %u") % index % size()).str()));
        }
        return m_value[index];
    }

    const variant& tuple::at(size_t index) const
    {
        if (index>size())
        {
            boost::throw_exception(variant_error((boost::format("Index %u is out of range for tuple of size %u") % index % size()).str()));
        }
        return m_value[index];
    }

    size_t tuple::size() const
    {
        return static_cast<size_t>(m_size);
    }

    bool tuple::empty() const
    {
        return m_size==0;
    }

    void tuple::clear()
    {
        boost::throw_exception(variant_error("Attempt to clear tuple"));
    }

    void tuple::swap(tuple& rhs)
	{
		std::swap(m_size, rhs.m_size);
		std::swap(m_value, rhs.m_value);
	}

	variant_const_iterator_base* tuple::begin() const
    {
        return new tuple_const_iterator(m_value);
    }
    variant_const_iterator_base* tuple::end() const
    {
        return new tuple_const_iterator(m_value+size());
    }
    variant_iterator_base* tuple::begin()
    {
        return new tuple_iterator(m_value);
    }
    variant_iterator_base* tuple::end()
    {
        return new tuple_iterator(m_value+size());
    }

}} // namespace protean::detail
