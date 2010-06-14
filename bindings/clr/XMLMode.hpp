//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CLR_XML_MODE_HPP
#define PROTEAN_CLR_XML_MODE_HPP

#include <protean/xml_common.hpp>

namespace protean { namespace clr {

    public enum class XMLMode
    {
        None        = protean::xml_mode::None,
        Preserve    = protean::xml_mode::Preserve,
        NoHeader    = protean::xml_mode::NoHeader,
        Indent      = protean::xml_mode::Indent,
        StripSpace  = protean::xml_mode::StripSpace,
        //CreateProxy = protean::xml_mode::CreateProxy, // not supported in CLR yet
        Default     = protean::xml_mode::Default
    };

}}

#endif // PROTEAN_CLR_XML_MODE_HPP
