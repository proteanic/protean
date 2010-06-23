//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/detail/dictionary.hpp>
#include <protean/detail/dictionary_iterator.hpp>
#include <protean/detail/hash.hpp>
#include <protean/variant.hpp>
#include <protean/variant_error.hpp>

namespace protean { namespace detail {

    int dictionary::compare(const collection& rhs) const
    {
        const dictionary* cast_rhs = dynamic_cast<const dictionary*>(&rhs);
        if (cast_rhs==NULL)
        {
            boost::throw_exception(variant_error("Unable to cast collection to dictionary"));
        }
        return (std::lexicographical_compare(m_value.begin(), m_value.end(), cast_rhs->m_value.begin(), cast_rhs->m_value.end()) ?
            -1 : (std::lexicographical_compare(cast_rhs->m_value.begin(), cast_rhs->m_value.end(), m_value.begin(), m_value.end()) ? 1 : 0));
    }

    boost::uint64_t dictionary::hash(boost::uint64_t seed) const
    {
        return hash_range(m_value.begin(), m_value.end(), seed);
    }

    bool dictionary::empty() const
    {
        return m_value.empty();
    }
    size_t dictionary::size() const
    {
        return m_value.size();
    }
    void dictionary::clear()
    {
        m_value.clear();
    }
    variant& dictionary::insert(const std::string& key, const variant& value)
    {
        if (has_key(key))
        {
            boost::throw_exception(variant_error(std::string("Attempt to insert duplicate key '") + key + "' into Dictionary"));
        }
        std::pair<container_t::iterator, bool> result = m_value.insert( make_pair( key, value ) );
        return result.first->second;
    }
    bool dictionary::has_key(const std::string& key) const
    {
        return m_value.find(key)!=m_value.end();
    }
    const variant& dictionary::at(const std::string& key) const
    {
        container_t::const_iterator it = m_value.find(key);
        if ( it==m_value.end() )
        {
            boost::throw_exception(variant_error(std::string("Item '") + key + "' not found in Dictionary"));
        }
        return it->second;
    }
    variant& dictionary::at(const std::string& key)
    {
        container_t::iterator it = m_value.find(key);
        if ( it==m_value.end() )
        {
            boost::throw_exception(variant_error(std::string("Item '") + key + "' not found in Dictionary"));
        }
        return it->second;
    }
    void dictionary::remove(const std::string& key)
    {
        container_t::iterator it = m_value.find(key);
        if ( it==m_value.end() )
        {
            boost::throw_exception(variant_error(std::string("Item '") + key + "' not found in Dictionary"));
        }
        m_value.erase(it);
    }
    variant dictionary::range(const std::string& key) const
    {
        variant result(variant::List);
        container_t::const_iterator it = m_value.find(key);
        if (it!=m_value.end())
        {
            result.push_back(it->second);
        }
        return result;
    }
    variant_const_iterator_base* dictionary::begin() const
    {
        return new dictionary_const_iterator(m_value.begin());
    }
    variant_const_iterator_base* dictionary::end() const
    {
        return new dictionary_const_iterator(m_value.end());
    }
    variant_iterator_base* dictionary::begin()
    {
        return new dictionary_iterator(m_value.begin());
    }
    variant_iterator_base* dictionary::end()
    {
        return new dictionary_iterator(m_value.end());
    }

}} // namespace protean::detail
