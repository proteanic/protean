//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_XML_COMMON_HPP
#define PROTEAN_XML_COMMON_HPP

#include <protean/config.hpp>

#if defined(_MSC_VER)
#if defined(PROTEAN_SOURCE) || (!defined(PROTEAN_DYN_LINK) && !defined(PROTEAN_NO_AUTOLINK))
#    pragma comment(lib, XERCES_BINARY ".lib")
#endif
#endif

namespace protean {

    struct xml_mode
    {
        enum
        {
            // General
            None                = 0x00000000,
            NoHeader            = 0x00000001,     // xml_writer: do not output XML header

            // Default mode
            CreateProxy         = 0x00000002,     // xml_reader: create proxy object if class has not been registered in factory
            
            // Preserve mode
            Preserve            = 0x00000008,     // xml_reader/xml_writer: preserve structure of XML
            StripSpace          = 0x00000010,     // xml_reader: strip whitespace-only text
            Indent              = 0x00000020,     // xml_writer: insert indentation where no text node is present

            Default             = None
        };
    };

  
    extern PROTEAN_DECL const char* xml_default_element;

    extern PROTEAN_DECL const char* xml_text;
    extern PROTEAN_DECL const char* xml_cdata;
    extern PROTEAN_DECL const char* xml_comment;
    extern PROTEAN_DECL const char* xml_instruction;
    extern PROTEAN_DECL const char* xml_attributes;
    extern PROTEAN_DECL const char* xml_target;
    extern PROTEAN_DECL const char* xml_data;
  

} // namespace protean

#endif // PROTEAN_XML_COMMON_HPP
