//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DETAIL_XML_UTILITY_HPP
#define PROTEAN_DETAIL_XML_UTILITY_HPP

#include <protean/config.hpp>

#include <protean/detail/xerces_include.hpp>

#include <string>

namespace protean {
    
    class variant;

    namespace detail {

        struct xml_utility
        {
            static bool is_ws(const XMLCh* const arg);
            static std::string transcode(const XMLCh* const arg);
            static void get_attributes(const xercesc::Attributes& attrs, variant& result);
            static std::string escape_chars(const std::string& arg);
            static void check_invalid_chars(const std::string& arg);
        };

}} // protean::detail

#endif // PROTEAN_DETAIL_XML_UTILITY_HPP
