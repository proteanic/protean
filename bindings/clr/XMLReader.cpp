//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#if defined(_MSC_VER)
#    pragma warning(disable:4503 4996)
#endif

#include "XMLReader.hpp"
#include "VariantException.hpp"
#include "StringTranslator.hpp"

namespace protean { namespace clr {

    XMLReader::XMLReader(System::IO::TextReader^ reader, XMLMode mode) :
        m_mode(mode),
        m_reader(new boost::iostreams::stream<TextReader>(reader))
    {
		m_entityReaders = gcnew System::Collections::Generic::Dictionary<System::String^,System::IO::TextReader^>();
	}

    XMLReader::XMLReader(System::IO::TextReader^ reader) :
        m_mode(XMLMode::Default),
        m_reader(new boost::iostreams::stream<TextReader>(reader))
    {
		m_entityReaders = gcnew System::Collections::Generic::Dictionary<System::String^,System::IO::TextReader^>();
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

        protean::xml_reader reader(*m_reader, (int)m_mode);
		if (m_entityPath!=nullptr)
		{
			reader.set_entity_path(StringTranslator(m_entityPath).c_str());
		}
		if (m_externalSchema!=nullptr)
		{
			reader.set_external_schema(StringTranslator(m_externalSchema).c_str());
		}

		std::vector<boost::shared_ptr<boost::iostreams::stream<TextReader>>> streams; // used for clearing up
		for each(System::Collections::Generic::KeyValuePair<System::String^, System::IO::TextReader^> kvp in m_entityReaders)
		{
			std::string id(StringTranslator(kvp.Key).c_str());
			boost::shared_ptr<boost::iostreams::stream<TextReader>> stream(new boost::iostreams::stream<TextReader>(kvp.Value));
			streams.push_back(stream);

			reader.add_entity_stream(id, *stream);
		}

        reader >> result->get_internals();

        return result;
        
        END_TRANSLATE_ERROR();
    }

    void XMLReader::SetEntityPath(System::String^ path)
	{
		m_entityPath = path;
	}
    void XMLReader::SetExternalSchema(System::String^ id)
	{
		m_externalSchema = id;
	}
    void XMLReader::AddEntityReader(System::String^ id, System::IO::TextReader^ reader)
	{
		m_entityReaders->Add(id, reader);
	}


}} // protean::clr
