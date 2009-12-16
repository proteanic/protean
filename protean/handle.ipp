//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

namespace protean {

    template<typename T>
    handle<T>::handle() :
        m_pointee(NULL)
    {
    }
    
    template<typename T>
    handle<T>::handle(T* pointee) :
        m_pointee(pointee)
    {
        if (m_pointee!=NULL)
        {
            ++m_pointee->m_ref_count;
        }
    }

    template<typename T>
    handle<T>::~handle()
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

    template<typename T>
    handle<T>::handle(const handle<T>& rhs)
    {
        m_pointee = rhs.m_pointee;
        if (m_pointee!=NULL)
        {
            ++m_pointee->m_ref_count;
        }
    }

    template<typename T>
    handle<T>& handle<T>::operator=(const handle<T>& rhs)
    {
        handle<T>(rhs).swap(*this);
        return *this;
    }

    template<typename T>
    T& handle<T>::operator*() const
    {
        if (m_pointee==NULL)
        {
            boost::throw_exception(variant_error("Attempt to dereference NULL handle"));
        }
        return *m_pointee;
    }

    template<typename T>
    T* handle<T>::operator->() const
    {
        if (m_pointee==NULL)
        {
            boost::throw_exception(variant_error("Attempt to dereference NULL handle"));
        }
        return m_pointee;
    }

    template<typename T>
    bool handle<T>::unique() const
    {
        return m_pointee!=NULL && m_pointee->m_ref_count==1;
    }

    template<typename T>
    bool handle<T>::null() const
    {
        return m_pointee==NULL;
    }

    template<typename T>
    void handle<T>::swap(handle<T>& rhs)
    {
        std::swap(m_pointee, rhs.m_pointee);
    }

    template<typename T>
    template<typename U>
    bool handle<T>::is() const
    {
        return as<U>()!=NULL;
    }

    template<typename T>
    template<typename U>
    U* handle<T>::as() const
    {
        return dynamic_cast<U*>(m_pointee);
    }

    template<typename T>
    template<typename U>
    handle<T> handle<T>::create()
    {
        return handle<T>(new U());
    }

} // namespace protean
