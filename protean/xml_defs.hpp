//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_XML_DEFS_HPP
#define PROTEAN_XML_DEFS_HPP

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
            None        = 0x00000000,
            Preserve    = 0x00000001,   // xml_reader/xml_writer: preserve structure of XML
            NoHeader    = 0x00000002,   // xml_writer: do not output XML header
            CreateProxy = 0x00000004,   // xml_reader: create proxy object if class has not been registered in factory
            Default     = None
        };
    };

    static const char* xml_default_element = "Variant";

} // namespace protean

#endif // PROTEAN_XML_DEFS_HPP
