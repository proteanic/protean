//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/object_handle.hpp>
#include <protean/variant_error.hpp>

namespace protean {

    object_handle::object_handle() :
        m_pointee(NULL)
    {
    }
    object_handle::object_handle(object* pointee) :
        m_pointee(pointee)
    {
        if (m_pointee!=NULL)
        {
            ++m_pointee->m_ref_count;
        }
    }
    object_handle::~object_handle()
    {
        if (m_pointee!=NULL)
        {
            if (--m_pointee->m_ref_count==0)
            {
                delete m_pointee;
            }
        }
        m_pointee = NULL;
    }
    object_handle::object_handle(const object_handle& rhs)
    {
        m_pointee = rhs.m_pointee;
        if (m_pointee!=NULL)
        {
            ++m_pointee->m_ref_count;
        }
    }
    object_handle& object_handle::operator=(const object_handle& rhs)
    {
        object_handle(rhs).swap(*this);
        return *this;
    }

    object& object_handle::operator*() const
    {
        if (m_pointee==NULL)
        {
            boost::throw_exception(variant_error("Attempt to dereference NULL handle"));
        }
        return *m_pointee;
    }

    object* object_handle::operator->() const
    {
        if (m_pointee==NULL)
        {
            boost::throw_exception(variant_error("Attempt to dereference NULL handle"));
        }
        return m_pointee;
    }

    bool object_handle::unique() const
    {
        return m_pointee!=NULL && m_pointee->m_ref_count==1;
    }

    object_handle::operator bool() const
    {
        return m_pointee==NULL;
    }

    void object_handle::swap(object_handle& rhs)
    {
        std::swap(m_pointee, rhs.m_pointee);
    }
} // namespace protean
