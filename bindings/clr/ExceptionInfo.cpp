//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include "ExceptionInfo.hpp"
#include "VariantException.hpp"

#include <sstream>

using namespace System::Runtime::InteropServices;

namespace protean { namespace clr {

    ExceptionInfo::ExceptionInfo(System::String^ type, System::String^ message)
    {
        BEGIN_TRANSLATE_ERROR();

        System::IntPtr type_handle = Marshal::StringToHGlobalAnsi(type);
        System::IntPtr message_handle = Marshal::StringToHGlobalAnsi(message);

        try
        {
            const char* type = static_cast<const char*>(type_handle.ToPointer());
            const char* message = static_cast<const char*>(message_handle.ToPointer());

            m_exception_info = new protean::exception_data(type, message);
        }
        finally
        {
            Marshal::FreeHGlobal(type_handle);
            Marshal::FreeHGlobal(message_handle);
        }

        END_TRANSLATE_ERROR();
    }

    ExceptionInfo::ExceptionInfo(System::String^ type, System::String^ message, System::String^ source, System::String^ stack)
    {
        BEGIN_TRANSLATE_ERROR();

        System::IntPtr type_handle = Marshal::StringToHGlobalAnsi(type);
        System::IntPtr message_handle = Marshal::StringToHGlobalAnsi(message);
        System::IntPtr source_handle = Marshal::StringToHGlobalAnsi(source);
        System::IntPtr stack_handle = Marshal::StringToHGlobalAnsi(stack);

        try
        {
            const char* type = static_cast<const char*>(type_handle.ToPointer());
            const char* message = static_cast<const char*>(message_handle.ToPointer());
            const char* source = static_cast<const char*>(source_handle.ToPointer());
            const char* stack = static_cast<const char*>(stack_handle.ToPointer());

            m_exception_info = new protean::exception_data(type, message, source, stack);
        }
        finally
        {
            Marshal::FreeHGlobal(type_handle);
            Marshal::FreeHGlobal(message_handle);
            Marshal::FreeHGlobal(source_handle);
            Marshal::FreeHGlobal(stack_handle);
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

}} // protean::clr
