//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#if defined(_MSC_VER)
#    pragma warning(disable:4996)
#endif

#include "BinaryWriter.hpp"
#include "VariantException.hpp"

using namespace System::Runtime::InteropServices;

namespace protean { namespace clr {

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

	array<System::Byte>^ BinaryWriter::ToBytes(Variant^ v)
	{
		return BinaryWriter::ToBytes(v, EnumFlag::None);
	}

	array<System::Byte>^ BinaryWriter::ToBytes(Variant^ v, EnumFlag flags)
	{
        BEGIN_TRANSLATE_ERROR();

		std::ostringstream oss;
        protean::binary_writer writer(oss, static_cast<protean::binary_writer::enum_flag_t>(flags));
		writer << v->get_internals();

		array<System::Byte>^ result = gcnew array<System::Byte>(static_cast<int>(oss.str().size()));
		Marshal::Copy((System::IntPtr)const_cast<char*>(oss.str().c_str()), result, 0, static_cast<int>(oss.str().size()));
	
		return result;

        END_TRANSLATE_ERROR();
	}


}} // protean::clr
