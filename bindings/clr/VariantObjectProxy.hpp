//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CLR_VARIANT_OBJECT_PROXY_HPP
#define PROTEAN_CLR_VARIANT_OBJECT_PROXY_HPP

#include "Variant.hpp"
#include "IVariantObject.hpp"

namespace protean { namespace clr {

    [System::Reflection::DefaultMember("ClassName")]
    public ref class VariantObjectProxy : public IVariantObject
    {
    public:
        VariantObjectProxy(System::String^ classname);
        ~VariantObjectProxy();
        !VariantObjectProxy();

        property System::String^ ClassName 
        {
            virtual System::String^ get();
        }
        property System::Int32 Version
        {
            virtual System::Int32 get();
        }
        
        virtual void Inflate(Variant^ params, System::Int32 version);
        
        virtual Variant^ Deflate();

        virtual System::String^ ToString() override;

    internal:
        VariantObjectProxy(const protean::object_proxy& arg);

        protean::object_proxy& get_internals();

    private:
        protean::object_proxy*    m_proxy;
    };

}} // protean::clr

#endif // PROTEAN_CLR_VARIANT_OBJECT_PROXY_HPP
