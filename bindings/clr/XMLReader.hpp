//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CLR_XML_READER_HPP
#define PROTEAN_CLR_XML_READER_HPP

#include "XMLMode.hpp"
#include "XMLConst.hpp"
#include "TextReader.hpp"
#include "Variant.hpp"

#include <protean/xml_reader.hpp>

namespace protean { namespace clr {

    public ref class XMLReader
    { 
    public:
        XMLReader(System::IO::TextReader^ reader);
        XMLReader(System::IO::TextReader^ reader, XMLMode mode);

        ~XMLReader();
        !XMLReader();

        Variant^ Read();

        void SetEntityPath(System::String^ path);
        void SetExternalSchema(System::String^ id);
        void AddEntityReader(System::String^ id, System::IO::TextReader^ reader);

    private:
        boost::iostreams::stream<TextReader>*   m_reader;
        XMLMode                                 m_mode;

		System::String^							m_entityPath;
		System::String^							m_externalSchema;
		System::Collections::Generic::Dictionary<System::String^,System::IO::TextReader^>^ m_entityReaders;

    };

}} // protean::clr

#endif // PROTEAN_CLR_XML_READER_HPP
