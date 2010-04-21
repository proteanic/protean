//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include "VariantObjectProxy.hpp"

using namespace System::Runtime::InteropServices;

namespace protean { namespace clr {

    VariantObjectProxy::VariantObjectProxy(System::String^ className)
    {
        System::IntPtr className_handle = Marshal::StringToHGlobalAnsi(className);

        try
        {
            const char* className_str = static_cast<const char*>(className_handle.ToPointer());
            m_proxy = new protean::object_proxy(className_str);
        }
        catch(System::Exception^)
        {
            throw;
        }
        finally
        {
            Marshal::FreeHGlobal(className_handle);
        }
    }

    VariantObjectProxy::VariantObjectProxy(const protean::object_proxy& arg) :
        m_proxy(new protean::object_proxy(arg))
    {
    }

    VariantObjectProxy::~VariantObjectProxy()
    {
        this->!VariantObjectProxy();
    }

    VariantObjectProxy::!VariantObjectProxy()
    {
        delete m_proxy;
    }

    System::String^ VariantObjectProxy::ClassName::get()
    {
        return gcnew System::String(m_proxy->name().c_str());
    }

    System::UInt32 VariantObjectProxy::Version::get()
    {
        return m_proxy->version();
    }

    Variant^ VariantObjectProxy::Deflate()
    {
        protean::variant params;
        m_proxy->deflate(params);

        return gcnew Variant(params);
    }

    void VariantObjectProxy::Inflate(Variant^ params, System::UInt32 version)
    {    
        m_proxy->inflate(params->get_internals(), version);
    }

    protean::object_proxy& VariantObjectProxy::get_internals()
    {
        return *m_proxy;
    }

    System::String^ VariantObjectProxy::ToString()
    {
        return gcnew System::String(m_proxy->name().c_str());
    }

}} // protean::clr
