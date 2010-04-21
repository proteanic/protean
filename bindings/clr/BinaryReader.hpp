//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CLR_BINARY_READER_HPP
#define PROTEAN_CLR_BINARY_READER_HPP

#include "BinaryStream.hpp"
#include "Variant.hpp"

#include <boost/iostreams/stream.hpp>

#include <protean/binary_reader.hpp>

namespace protean { namespace clr {

	public ref class BinaryReader
	{
	private:
		typedef BinaryStream<boost::iostreams::source_tag> stream_type;

	public:
		BinaryReader(System::IO::Stream^ stream);

		~BinaryReader();
		!BinaryReader();

		Variant^ Read();

	private:
		std::istream* m_stream;
	};

}} // protean::clr

#endif //PROTEAN_CLR_BINARY_READER_HPP
