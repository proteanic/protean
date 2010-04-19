//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CLR_TEXT_READER_HPP
#define PROTEAN_CLR_TEXT_READER_HPP

#include <vcclr.h>

#include <iostream>
#include <boost/iostreams/stream.hpp>

namespace protean { namespace clr {

	class TextReader
	{
	public:
		typedef char char_type;
		typedef boost::iostreams::source_tag category;

	public:
		TextReader(System::IO::TextReader^ reader);

		std::streamsize read(char* str, std::streamsize n);

	private:
		gcroot<System::IO::TextReader^>	m_reader;
	};

}} // protean::clr

#endif //PROTEAN_CLR_TEXT_READER_HPP

