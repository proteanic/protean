//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/detail/bag.hpp>
#include <protean/detail/bag_iterator.hpp>
#include <protean/variant.hpp>
#include <protean/variant_error.hpp>

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/functional/hash.hpp>

namespace protean { namespace detail {

    int bag::compare(const collection& rhs) const
    {
        const bag* cast_rhs = dynamic_cast<const bag*>(&rhs);
        if (cast_rhs==NULL)
        {
            boost::throw_exception(variant_error("Unable to cast collection to bag"));
        }
        if ( m_value.size()!=cast_rhs->m_value.size() )
        {
            return m_value.size()<cast_rhs->m_value.size();
        }
        return std::lexicographical_compare(
            m_value.begin(), m_value.end(), cast_rhs->m_value.begin(), cast_rhs->m_value.end() );
    }

    size_t bag::hash() const
    {
        return boost::hash_range(m_value.begin(), m_value.end());
    }

    bool bag::empty() const
    {
        return m_value.empty();
    }
    size_t bag::size() const
    {
        return m_value.size();
    }
    void bag::clear()
    {
        m_value.clear();
    }
    variant& bag::insert(const std::string& key, const variant& value)
    {
        m_value.push_back( make_pair( key, value ) );
        return m_value.rbegin()->second;
    }
    bool bag::has_key(const std::string& key) const
    {
        return (find_impl(key)!=m_value.end());
    }
    const variant& bag::at(const std::string& key) const
    {
        container_type::const_iterator itr(find_impl(key));
        if (itr==m_value.end())
        {
            boost::throw_exception(variant_error(std::string("Item '") + key + "' not found in Bag"));
        }
        return itr->second;
    }
    variant& bag::at(const std::string& key)
    {
        container_type::iterator it(find_impl(key));
        if ( it==m_value.end() )
        {
            boost::throw_exception(variant_error(std::string("Item '") + key + "' not found in Bag"));
        }
        return it->second;
    }
    void bag::remove(const std::string& key)
    {
        container_type::iterator it(m_value.begin());
        while(it!=m_value.end())
        {
            if(it->first == key)
                it = m_value.erase(it);
            else
                it++;
        }
    }
    variant bag::range(const std::string& key) const
    {
        variant result(variant::List);
        container_type::const_iterator it(m_value.begin());
        while(it!=m_value.end())
        {
            if(it->first == key)
                result.push_back(it->second);
            ++it;
        }

        return result;
    }
    variant_const_iterator_base* bag::begin() const
    {
        return new bag_const_iterator(m_value.begin());
    }
    variant_const_iterator_base* bag::end() const
    {
        return new bag_const_iterator(m_value.end());
    }
    variant_iterator_base* bag::begin()
    {
        return new bag_iterator(m_value.begin());
    }
    variant_iterator_base* bag::end()
    {
        return new bag_iterator(m_value.end());
    }

    bag::container_type::const_iterator bag::find_impl(const std::string& key) const
    {
        container_type::const_iterator itr(
            std::find_if(
                m_value.begin(), 
                m_value.end(), 
                boost::lambda::bind(&container_type::value_type::first, boost::lambda::_1) == key
            )
        );
        return itr;
    }

    bag::container_type::iterator bag::find_impl(const std::string& key)
    {
        container_type::iterator itr(
            std::find_if(
                m_value.begin(), 
                m_value.end(), 
                boost::lambda::bind(&container_type::value_type::first, boost::lambda::_1) == key
            )
        );
        return itr;
    }

}} // namespace protean::detail
