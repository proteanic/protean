//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CLR_BINARY_READER_HPP
#define PROTEAN_CLR_BINARY_READER_HPP

#include "BinaryMode.hpp"
#include "BinaryStream.hpp"
#include "Variant.hpp"

#if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable:4244)
#endif

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

        static Variant^ FromBytes(array<System::Byte>^ bytes);

    private:
        std::istream* m_stream;
    };

}} // protean::clr

#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#endif //PROTEAN_CLR_BINARY_READER_HPP
