//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CLR_BINARY_WRITER_HPP
#define PROTEAN_CLR_BINARY_WRITER_HPP

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

        enum class EnumFlag
        {
            None        = protean::binary_writer::None,
            Compress    = protean::binary_writer::Compress
        };

        BinaryWriter(System::IO::Stream^ stream);

        ~BinaryWriter();
        !BinaryWriter();

        void Write(Variant^ v);
        void Write(Variant^ v, EnumFlag flags);

        static array<System::Byte>^ ToBytes(Variant^ v, EnumFlag flags);
        static array<System::Byte>^ ToBytes(Variant^ v);

    private:
        std::ostream* m_stream;
        EnumFlag m_flags;
    };

}} // protean::clr

#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#endif // PROTEAN_CLR_BINARY_WRITER_HPP
