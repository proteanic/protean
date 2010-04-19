//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CLR_BINARY_STREAM_HPP
#define PROTEAN_CLR_BINARY_STREAM_HPP

#include <vcclr.h>
#include <iostream>

namespace protean { namespace clr {

	template<typename Category>
	class BinaryStream
	{
	public:
		typedef char char_type;
		typedef Category category;

		BinaryStream(System::IO::Stream^ stream);

		std::streamsize read(char* s, std::streamsize n);
		std::streamsize write(const char* s, std::streamsize n);

	private:
		gcroot<System::IO::Stream^>		m_stream;
	};

}} // protean::clr

#endif // PROTEAN_CLR_BINARY_STREAM_HPP
