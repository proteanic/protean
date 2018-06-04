#include <protean/variant_error.hpp>

namespace protean {

    template<typename T>
    handle<T>::handle() :
        m_pointee(nullptr)
    {
    }
    
    template<typename T>
    handle<T>::handle(T* pointee) :
        m_pointee(pointee)
    {
        if (m_pointee!=nullptr)
        {
            ++m_pointee->m_ref_count;
        }
    }

    template<typename T>
    handle<T>::~handle()
    {
        if (m_pointee!=nullptr)
        {
            if (--m_pointee->m_ref_count==0)
            {
                delete m_pointee;
            }
        }
        m_pointee = nullptr;
    }

    template<typename T>
    handle<T>::handle(const handle<T>& rhs)
    {
        m_pointee = rhs.m_pointee;
        if (m_pointee!=nullptr)
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
        if (m_pointee==nullptr)
        {
            boost::throw_exception(variant_error("Attempt to dereference nullptr handle"));
        }
        return *m_pointee;
    }

    template<typename T>
    T* handle<T>::operator->() const
    {
        if (m_pointee==nullptr)
        {
            boost::throw_exception(variant_error("Attempt to dereference nullptr handle"));
        }
        return m_pointee;
    }

    template<typename T>
    bool handle<T>::unique() const
    {
        return m_pointee!=nullptr && m_pointee->m_ref_count==1;
    }

    template<typename T>
    bool handle<T>::null() const
    {
        return m_pointee==nullptr;
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
        return as<U>()!=nullptr;
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
