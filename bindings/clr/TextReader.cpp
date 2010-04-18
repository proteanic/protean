//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include "TextReader.hpp"

namespace Protean { namespace CLR {

	TextReader::TextReader(System::IO::TextReader^ reader) :
		m_reader(reader)
	{
	}

	std::streamsize TextReader::read(char* str, std::streamsize n)
	{
		array<System::Char>^ chars = gcnew array<System::Char>(static_cast<int>(n));

		int count = m_reader->Read(chars, 0, (int)n);

        pin_ptr<System::Char> chars_ptr = &(chars[0]);

		strncpy_s(str, count, reinterpret_cast<char*>(chars_ptr), count);

		return count;
	}

}} // Protean::CLR
