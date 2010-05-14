//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CLR_BINARY_WRITER_HPP
#define PROTEAN_CLR_BINARY_WRITER_HPP

#include "BinaryMode.hpp"
#include "BinaryStream.hpp"
#include "Variant.hpp"

#if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable:4244)
#endif

#include <boost/iostreams/stream.hpp>

#include <protean/binary_writer.hpp>

namespace protean { namespace clr {

    public ref class BinaryWriter
    {
    private:
        typedef BinaryStream<boost::iostreams::sink_tag> stream_type;
    public:

        BinaryWriter(System::IO::Stream^ stream);
        BinaryWriter(System::IO::Stream^ stream, BinaryMode mode);

        ~BinaryWriter();
        !BinaryWriter();

        void Write(Variant^ v);
        static array<System::Byte>^ ToBytes(Variant^ v);
        static array<System::Byte>^ ToBytes(Variant^ v, BinaryMode mode);

    private:
        std::ostream*   m_stream;
        BinaryMode      m_mode;
    };

}} // protean::clr

#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#endif // PROTEAN_CLR_BINARY_WRITER_HPP
