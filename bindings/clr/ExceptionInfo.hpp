//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CLR_EXCEPTION_INFO_HPP
#define PROTEAN_CLR_EXCEPTION_INFO_HPP

#include <protean/exception_data.hpp>

namespace protean { namespace clr {

    [System::Reflection::DefaultMember("Message")]
    public ref class ExceptionInfo
    {
    public:
        ExceptionInfo(System::String^ type, System::String^ message);
        ExceptionInfo(System::String^ type, System::String^ message, System::String^ source, System::String^ stack);

        ~ExceptionInfo();
        !ExceptionInfo();

        property System::String^ Type
        {
            System::String^ get();
        }

        property System::String^ Message
        {
            System::String^ get();
        }

        property System::String^ Source
        {
            System::String^ get();
        }

        property System::String^ Stack
        {
            System::String^ get();
        }

        
        virtual System::String^ ToString() override;

    internal:

        explicit ExceptionInfo(const protean::exception_data& rhs);

        protean::exception_data&    get_internals();

    private:
        protean::exception_data*    m_exception_info;
    };

}} // protean::clr

#endif //PROTEAN_CLR_EXCEPTION_INFO_HPP
