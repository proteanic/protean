//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CLR_TEXT_WRITER_HPP
#define PROTEAN_CLR_TEXT_WRITER_HPP

#include <vcclr.h>

#include <iostream>
#include <boost/iostreams/stream.hpp>

namespace protean { namespace clr {

	class TextWriter
	{
	public:
		typedef char char_type;
		typedef boost::iostreams::sink_tag category;

	public:
		TextWriter(System::IO::TextWriter^ writer);

		std::streamsize write(const char* s, std::streamsize n);

	private:
		gcroot<System::IO::TextWriter^>	m_writer;
	};

}} // protean::clr

#endif //PROTEAN_CLR_TEXT_WRITER_HPP

