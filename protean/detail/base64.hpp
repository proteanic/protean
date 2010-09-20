//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_DETAIL_BASE64_HPP
#define PROTEAN_DETAIL_BASE64_HPP

namespace protean { namespace detail {

    char* b64_encode(const char* input_data, unsigned int input_length, unsigned int* output_length);
    char* b64_decode(const char* input_data, unsigned int input_length, unsigned int* output_length);
}}

#endif
