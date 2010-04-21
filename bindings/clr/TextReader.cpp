//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include "TextReader.hpp"

using namespace System::Runtime::InteropServices;

using namespace System::Text;

namespace protean { namespace clr {

    TextReader::TextReader(System::IO::TextReader^ reader) :
        m_reader(reader)
    {
    }

    std::streamsize TextReader::read(char* str, std::streamsize n)
    {
        array<System::Char>^ chars = gcnew array<System::Char>(static_cast<int>(n));

        pin_ptr<System::Char> ptr = &chars[0];

        int count = m_reader->Read(chars, 0, (int)n);

        ASCIIEncoding^ ascii = gcnew ASCIIEncoding();

        return ascii->GetBytes(reinterpret_cast<wchar_t*>(ptr), count, reinterpret_cast<unsigned char*>(str), count);

        return count;
    }

}} // protean::clr
