//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#if defined(_MSC_VER)
#    pragma warning(disable:4503 4996)
#endif

#include "BinaryWriter.hpp"
#include "VariantException.hpp"
#include "StrongReference.hpp"

namespace protean { namespace clr {

    BinaryWriter::BinaryWriter(System::IO::Stream^ stream) :
        m_mode(BinaryMode::Default),
        m_stream(new boost::iostreams::stream<stream_type>(stream))
    {
    }

    BinaryWriter::BinaryWriter(System::IO::Stream^ stream, BinaryMode mode) :
        m_mode(mode),
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
        BEGIN_TRANSLATE_ERROR();

        STRONG_REFERENCE(this);
        STRONG_REFERENCE(v);

        protean::binary_writer writer(*m_stream, (int)m_mode);
        writer << v->get_internals();
        m_stream->flush();

        END_TRANSLATE_ERROR();
    }

    array<System::Byte>^ BinaryWriter::ToBytes(Variant^ v)
    {
        return ToBytes(v, BinaryMode::Default);
    }

    array<System::Byte>^ BinaryWriter::ToBytes(Variant^ v, BinaryMode mode)
    {
        BEGIN_TRANSLATE_ERROR();

        STRONG_REFERENCE(v);

        std::ostringstream oss;
        protean::binary_writer writer(oss, (int)mode);
        writer << v->get_internals();

        array<System::Byte>^ result = gcnew array<System::Byte>(static_cast<int>(oss.str().size()));
        Marshal::Copy((System::IntPtr)const_cast<char*>(oss.str().c_str()), result, 0, static_cast<int>(oss.str().size()));
    
        return result;

        END_TRANSLATE_ERROR();
    }


}} // protean::clr
