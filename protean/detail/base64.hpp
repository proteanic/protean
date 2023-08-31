#ifndef PROTEAN_DETAIL_BASE64_HPP
#define PROTEAN_DETAIL_BASE64_HPP

namespace protean { namespace detail {

    char* b64_encode(const char* input_data, unsigned int input_length, unsigned int* output_length);
    char* b64_decode(const char* input_data, unsigned int input_length, unsigned int* output_length);
}}

#endif
