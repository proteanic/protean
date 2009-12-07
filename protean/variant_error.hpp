//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_VARIANT_ERROR_HPP
#define PROTEAN_VARIANT_ERROR_HPP

#include <protean/config.hpp>

#include <boost/throw_exception.hpp>
#include <boost/format.hpp>

namespace protean {

    class variant_error : public std::exception
    {
    public:
        variant_error(const std::string& what_arg) :
            std::exception(what_arg.c_str())
        {}
    };

} // namespace protean

#endif // PROTEAN_VARIANT_ERROR_HPP
