//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CLR_XML_WRITER_HPP
#define PROTEAN_CLR_XML_WRITER_HPP

#include "XMLMode.hpp"
#include "TextWriter.hpp"
#include "Variant.hpp"

#include <protean/xml_writer.hpp>

namespace protean { namespace clr {

    public ref class XMLWriter
    {
    public:
        XMLWriter(System::IO::TextWriter^ writer);
        XMLWriter(System::IO::TextWriter^ writer, XMLMode mode);

        ~XMLWriter();
        !XMLWriter();

        void Write(Variant^ v);

    private:
        boost::iostreams::stream<TextWriter>*   m_writer;
        XMLMode                                 m_mode;
    };

}} // protean::clr

#endif // PROTEAN_CLR_XML_WRITER_HPP
