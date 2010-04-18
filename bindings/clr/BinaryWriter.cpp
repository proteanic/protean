//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

#include "BinaryWriter.hpp"
#include "VariantException.hpp"

namespace Protean { namespace CLR {

    BinaryWriter::BinaryWriter(System::IO::Stream^ stream) :
        m_stream(new boost::iostreams::stream<stream_type>(stream))
	{
	}

    BinaryWriter::~BinaryWriter()
    {
		this->!BinaryWriter();
    }

    BinaryWriter::!BinaryWriter()
    {
        delete m_stream;
    }

    void BinaryWriter::Write(Variant^ v)
	{
        Write(v, EnumFlag::None);
    }

    void BinaryWriter::Write(Variant^ v, EnumFlag flags)
	{
        BEGIN_TRANSLATE_ERROR();

        protean::binary_writer writer(*m_stream, static_cast<protean::binary_writer::enum_flag_t>(flags));
		writer << v->get_internals();
		m_stream->flush();

        END_TRANSLATE_ERROR();
    }

}} // Protean::CLR
