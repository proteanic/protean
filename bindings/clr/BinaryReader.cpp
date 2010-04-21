//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#if defined(_MSC_VER)
#    pragma warning(disable:4996)
#endif

#include "BinaryReader.hpp"
#include "VariantException.hpp"

namespace protean { namespace clr {

    BinaryReader::BinaryReader(System::IO::Stream^ stream) :
		m_stream(new boost::iostreams::stream<stream_type>(stream))
	{
	}

    BinaryReader::~BinaryReader()
    {
		this->!BinaryReader();
    }

    BinaryReader::!BinaryReader()
    {
        delete m_stream;
    }

    Variant^ BinaryReader::Read()
	{
        BEGIN_TRANSLATE_ERROR();

	    Variant^ result = gcnew Variant();
	    protean::binary_reader reader(*m_stream);
	    reader >> result->get_internals();

	    return result;

        END_TRANSLATE_ERROR();
	}

}} // protean::clr
