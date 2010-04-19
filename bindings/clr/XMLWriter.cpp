//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

#include "XMLWriter.hpp"
#include "VariantException.hpp"

namespace protean { namespace clr {

    XMLWriter::XMLWriter(System::IO::TextWriter^ writer) :
		m_writer(new boost::iostreams::stream<TextWriter>(writer))
	{
	}

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
        Write(v, EnumFlag::None);
    }

    void XMLWriter::Write(Variant^ v, EnumFlag flags)
	{
        BEGIN_TRANSLATE_ERROR();

		protean::xml_writer writer(*m_writer, static_cast<protean::xml_writer::enum_flag_t>(flags));
		writer << v->get_internals();
		m_writer->flush();

        END_TRANSLATE_ERROR();
	}

}} // protean::clr
