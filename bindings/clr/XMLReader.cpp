//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#if defined(_MSC_VER)
#    pragma warning(disable:4996)
#endif

#include "XMLReader.hpp"
#include "VariantException.hpp"

namespace protean { namespace clr {

	XMLReader::XMLReader(System::IO::TextReader^ reader) :
		m_reader(new boost::iostreams::stream<TextReader>(reader))
	{
	}

    XMLReader::~XMLReader()
    {
		this->!XMLReader();
    }

    XMLReader::!XMLReader()
    {
        delete m_reader;
    }

    Variant^ XMLReader::Read()
    {
        BEGIN_TRANSLATE_ERROR();

		Variant^ result = gcnew Variant();

	    protean::xml_reader reader(*m_reader);
	    reader >> result->get_internals();

	    return result;
        
        END_TRANSLATE_ERROR();
    }

}} // protean::clr
