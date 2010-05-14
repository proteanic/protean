//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#if defined(_MSC_VER)
#    pragma warning(disable:4503 4996)
#endif

#include "XMLWriter.hpp"
#include "VariantException.hpp"

namespace protean { namespace clr {

    XMLWriter::XMLWriter(System::IO::TextWriter^ writer, XMLMode mode) :
        m_mode(mode),
        m_writer(new boost::iostreams::stream<TextWriter>(writer))
    { }

    XMLWriter::XMLWriter(System::IO::TextWriter^ writer) :
        m_mode(XMLMode::Default),
        m_writer(new boost::iostreams::stream<TextWriter>(writer))
    { }

    XMLWriter::~XMLWriter()
    {
        this->!XMLWriter();
    }

    XMLWriter::!XMLWriter()
    {
        delete m_writer;
    }

    void XMLWriter::Write(Variant^ v)
    {
        BEGIN_TRANSLATE_ERROR();

        protean::xml_writer writer(*m_writer, (int)m_mode);
        writer << v->get_internals();
        m_writer->flush();

        END_TRANSLATE_ERROR();
    }

}} // protean::clr
