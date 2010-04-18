//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include "TextWriter.hpp"

namespace protean { namespace clr {

	TextWriter::TextWriter(System::IO::TextWriter^ writer) :
		m_writer(writer)
	{
	}

	std::streamsize TextWriter::write(const char* s, std::streamsize n)
	{
		m_writer->Write(gcnew System::String(s, 0, static_cast<int>(n)));
		return n;
	}

}} // protean::clr
