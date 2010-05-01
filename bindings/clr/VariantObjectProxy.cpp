//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include "VariantObjectProxy.hpp"
#include "VariantException.hpp"
#include "StringTranslator.hpp"
#include "StrongReference.hpp"

namespace protean { namespace clr {

    VariantObjectProxy::VariantObjectProxy(System::String^ className)
    {
        BEGIN_TRANSLATE_ERROR();

        std::string className_str(StringTranslator(className).c_str());

        m_proxy = new protean::object_proxy(className_str);

        END_TRANSLATE_ERROR();
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
        BEGIN_TRANSLATE_ERROR();

		STRONG_REFERENCE(this);

        return gcnew System::String(m_proxy->name().c_str());

        END_TRANSLATE_ERROR();    
    }

    System::UInt32 VariantObjectProxy::Version::get()
    {
        BEGIN_TRANSLATE_ERROR();

		STRONG_REFERENCE(this);

		return m_proxy->version();

        END_TRANSLATE_ERROR();    
    }

    Variant^ VariantObjectProxy::Deflate()
    {
        BEGIN_TRANSLATE_ERROR();

		STRONG_REFERENCE(this);

		protean::variant params;
        m_proxy->deflate(params);

        return gcnew Variant(params);

        END_TRANSLATE_ERROR();    
    }

    void VariantObjectProxy::Inflate(Variant^ params, System::UInt32 version)
    {    
        BEGIN_TRANSLATE_ERROR();

		STRONG_REFERENCE(this);

		m_proxy->inflate(params->get_internals(), version);

        END_TRANSLATE_ERROR();    
    }

    protean::object_proxy& VariantObjectProxy::get_internals()
    {
        BEGIN_TRANSLATE_ERROR();

        return *m_proxy;

        END_TRANSLATE_ERROR();    
    }

    System::String^ VariantObjectProxy::ToString()
    {
        BEGIN_TRANSLATE_ERROR();

		STRONG_REFERENCE(this);

		return gcnew System::String(m_proxy->name().c_str());

        END_TRANSLATE_ERROR();    
    }

}} // protean::clr
