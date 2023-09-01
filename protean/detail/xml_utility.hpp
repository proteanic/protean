#ifndef PROTEAN_DETAIL_XML_UTILITY_HPP
#define PROTEAN_DETAIL_XML_UTILITY_HPP

#ifndef PROTEAN_DISABLE_XML

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

#endif // #ifndef PROTEAN_DISABLE_XML

#endif // #ifndef PROTEAN_DETAIL_XML_UTILITY_HPP
