//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/detail/xml_utility.hpp>
#include <protean/detail/xerces_include.hpp>
#include <protean/variant.hpp>

namespace protean { namespace detail {

    /*static*/ bool xml_utility::is_ws(const XMLCh* const arg)
    {
        return xercesc::XMLChar1_0::isAllSpaces(arg, xercesc::XMLString::stringLen(arg));
    }

    /*static*/ std::string xml_utility::transcode(const XMLCh* const arg)
    {
        char* transcoded = xercesc::XMLString::transcode(arg);
        std::string tmp(transcoded);
        xercesc::XMLString::release(&transcoded);

        return tmp;
    }

    /*static*/ void xml_utility::get_attributes(const xercesc::Attributes& attrs, variant& result)
    {
        for ( unsigned int i=0; i<attrs.getLength(); ++i )
        {
            std::string name  = transcode(attrs.getQName(i));
            std::string value = transcode(attrs.getValue(i));

            // make sure we remove any schema related attributes
            if (name.find("xsi")==std::string::npos)
            {
                result.insert(name, variant(variant::Any, value));
            }
        }
    }

    /*static*/ std::string xml_utility::escape_chars(const std::string& arg)
    {
        std::string tmp(arg);

        // make sure '&' is escaped first, as it is introduced by the others
        boost::algorithm::replace_all(tmp, "&",  "&amp;");
        boost::algorithm::replace_all(tmp, "<",  "&lt;");
        return tmp;
    }

    /*static*/ void xml_utility::check_invalid_chars(const std::string& arg)
    {
        if(arg.find_first_of("&<>'\" ") != std::string::npos)
        {
            boost::throw_exception(variant_error("Invalid character in string '" + arg + "'"));
        }
    }

}} // protean::detail
