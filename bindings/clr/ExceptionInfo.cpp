//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include "ExceptionInfo.hpp"
#include "VariantException.hpp"

#include <sstream>

using namespace System::Runtime::InteropServices;

namespace Protean { namespace CLR {

    ExceptionInfo::ExceptionInfo(System::String^ type, System::String^ message)
    {
        BEGIN_TRANSLATE_ERROR();

        System::IntPtr ip_type = Marshal::StringToHGlobalAnsi(type);
        System::IntPtr ip_message = Marshal::StringToHGlobalAnsi(message);

        try
        {
            const char* type = static_cast<const char*>(ip_type.ToPointer());
            const char* message = static_cast<const char*>(ip_message.ToPointer());

		    m_exception_info = new protean::exception_data(type, message);
        }
        finally
        {
            Marshal::FreeHGlobal(ip_type);
            Marshal::FreeHGlobal(ip_message);
        }

        END_TRANSLATE_ERROR();
    }

	ExceptionInfo::ExceptionInfo(System::String^ type, System::String^ message, System::String^ source, System::String^ stack)
	{
        BEGIN_TRANSLATE_ERROR();

        System::IntPtr ip_type = Marshal::StringToHGlobalAnsi(type);
        System::IntPtr ip_message = Marshal::StringToHGlobalAnsi(message);
        System::IntPtr ip_source = Marshal::StringToHGlobalAnsi(source);
        System::IntPtr ip_stack = Marshal::StringToHGlobalAnsi(stack);

        try
        {
            const char* type = static_cast<const char*>(ip_type.ToPointer());
            const char* message = static_cast<const char*>(ip_message.ToPointer());
            const char* source = static_cast<const char*>(ip_source.ToPointer());
            const char* stack = static_cast<const char*>(ip_stack.ToPointer());

		    m_exception_info = new protean::exception_data(type, message, source, stack);
        }
        finally
        {
            Marshal::FreeHGlobal(ip_type);
            Marshal::FreeHGlobal(ip_message);
            Marshal::FreeHGlobal(ip_source);
            Marshal::FreeHGlobal(ip_stack);
        }

        END_TRANSLATE_ERROR();
	}

    ExceptionInfo::ExceptionInfo(const protean::exception_data& rhs)
    {
	    m_exception_info = new protean::exception_data(rhs);
    }

	ExceptionInfo::~ExceptionInfo()
	{
		this->!ExceptionInfo();
	}

	ExceptionInfo::!ExceptionInfo()
	{
		delete m_exception_info;
	}

	System::String^ ExceptionInfo::Type::get()
	{
		return gcnew System::String(m_exception_info->type().c_str());
	}

	System::String^ ExceptionInfo::Message::get()
	{
		return gcnew System::String(m_exception_info->message().c_str());
	}

	System::String^ ExceptionInfo::Source::get()
	{
		return gcnew System::String(m_exception_info->source().c_str());
	}

	System::String^ ExceptionInfo::Stack::get()
	{
		return gcnew System::String(m_exception_info->stack().c_str());
	}

	protean::exception_data& ExceptionInfo::get_internals()
	{
		return *m_exception_info;
	}

	System::String^ ExceptionInfo::ToString()
	{
        std::ostringstream oss;
        oss << m_exception_info->type() << "(" << m_exception_info->message() << ")";

        if (!m_exception_info->source().empty())
        {
            oss << " in " << m_exception_info->source();
        }

        if (!m_exception_info->stack().empty())
        {
            oss << "\n" << m_exception_info->stack();
        }

		return gcnew System::String(oss.str().c_str());
	}

}} // Protean::CLR
