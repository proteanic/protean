//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CLR_XML_READER_HPP
#define PROTEAN_CLR_XML_READER_HPP

#include "TextReader.hpp"
#include "Variant.hpp"

#include <protean/xml_reader.hpp>

namespace Protean { namespace CLR {

	public ref class XMLReader
	{ 
	public:
		XMLReader(System::IO::TextReader^ reader);
		~XMLReader();
		!XMLReader();

		Variant^ Read();

	private:
		boost::iostreams::stream<TextReader>* m_reader;
	};

}} // Protean::CLR

#endif // PROTEAN_CLR_XML_READER_HPP
