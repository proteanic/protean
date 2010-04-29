//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CLR_STRING_TRANSLATOR_HPP
#define PROTEAN_CLR_STRING_TRANSLATOR_HPP

#include <boost/utility.hpp>

namespace protean { namespace clr {

	using namespace System::Runtime::InteropServices;

	class StringTranslator : boost::noncopyable
	{
	public:
		StringTranslator(System::String^ str)
		{
			m_handle = Marshal::StringToHGlobalAnsi(str);
		}
		~StringTranslator()
		{
			Marshal::FreeHGlobal(m_handle);
		}
		char* c_str()
		{
			return static_cast<char*>(m_handle.ToPointer());
		}

	private:
		System::IntPtr m_handle;
	};

}} //protean::clr

#endif // PROTEAN_CLR_STRING_TRANSLATOR_HPP
