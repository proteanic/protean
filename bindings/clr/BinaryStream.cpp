//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include "BinaryStream.hpp"

#include <boost/iostreams/stream.hpp>

using namespace System::Runtime::InteropServices;

namespace protean { namespace clr {

	template<typename Category>
	BinaryStream<Category>::BinaryStream(System::IO::Stream^ stream) :
		m_stream(stream)
	{
	}

	template<typename Category>
	std::streamsize BinaryStream<Category>::read(char* bytes, std::streamsize n)
	{
		array<System::Byte>^ byteBuffer = gcnew array<System::Byte>(static_cast<int>(n));

		int count = m_stream->Read(byteBuffer, 0, (int)n);

		pin_ptr<System::Byte> ptr = &byteBuffer[0];

		memcpy(bytes, reinterpret_cast<const char*>(ptr), count);

		return count;
	}

	template<typename Category>
	std::streamsize BinaryStream<Category>::write(const char* bytes, std::streamsize n)
	{
		array<System::Byte>^ byteBuffer = gcnew array<System::Byte>(static_cast<int>(n));
		Marshal::Copy((System::IntPtr)const_cast<char*>(bytes), byteBuffer, 0, static_cast<int>(n));
		m_stream->Write(byteBuffer, 0, static_cast<int>(n));
		return n;
	}

	template BinaryStream<boost::iostreams::source_tag>;
	template BinaryStream<boost::iostreams::sink_tag>;

}} // protean::clr
