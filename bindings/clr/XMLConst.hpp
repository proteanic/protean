//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CLR_XML_CONST_HPP
#define PROTEAN_CLR_XML_CONST_HPP

namespace protean { namespace clr {

    public ref class XMLConst abstract sealed
    {
    public:
        static const System::String^ Text = gcnew System::String(protean::xml_text);
        static const System::String^ Comment = gcnew System::String(protean::xml_comment);
        static const System::String^ Instruction = gcnew System::String(protean::xml_instruction);
        static const System::String^ Attributes = gcnew System::String(protean::xml_attributes);
        static const System::String^ Target = gcnew System::String(protean::xml_target);
        static const System::String^ Data = gcnew System::String(protean::xml_data);
    };

}}

#endif // PROTEAN_CLR_XML_CONST_HPP
