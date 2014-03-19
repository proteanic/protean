#include <protean/detail/base64.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/scoped_ptr.hpp>

#include <iostream>
#include <stdlib.h>

namespace protean { namespace detail {

    static const char base64_chars[] =
    {
	    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	    "abcdefghijklmnopqrstuvwxyz"
	    "0123456789+/"
    };

    static const char base64_inverse[] =
    {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0x3F,
        0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
        0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
        0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };

    const unsigned int quads_per_line = 15;

    static inline void split_octet_1(char ch, char& b1, char& b2) {
        b1 = ch >> 2;
        b2 = ( ch & 0x3 ) << 4;
    }

    static inline void split_octet_2(char ch, char& b2, char& b3) {
        b2 |= ch >> 4;  // combine with previous value
        b3 = ( ch & 0xf ) << 2;
    }

    static inline void split_octet_3(char ch, char& b3, char& b4) {
        b3 |= ch >> 6;  // combine with previous value
        b4 = ( ch & 0x3f );
    }

    char* b64_encode(const char* input_data, unsigned int input_length, unsigned int* output_length)
    {
        if (input_data==0 || output_length==0)
            return 0;

        int quad_count = ( (int)input_length + 2 ) / 3;
        if (quad_count==0)
            return 0;

        int line_count = (quad_count + quads_per_line-1) / quads_per_line;

        char  b1, b2, b3, b4;

        unsigned int input_index = 0;
        unsigned int output_index = 0;

        char* output_data = (char*)malloc(quad_count*4+line_count+1);

        for (int quad=1; quad<quad_count; ++quad)
        {
            split_octet_1(input_data[input_index++], b1, b2);
            split_octet_2(input_data[input_index++], b2, b3);
            split_octet_3(input_data[input_index++], b3, b4);

            output_data[output_index++] = base64_chars[b1];
            output_data[output_index++] = base64_chars[b2];
            output_data[output_index++] = base64_chars[b3];
            output_data[output_index++] = base64_chars[b4];

            if ((quad % quads_per_line) == 0 )
            {
                output_data[output_index++] = '\n';
            }
        }

        split_octet_1(input_data[input_index++], b1, b2);
        output_data[output_index++] = base64_chars[b1];

        if(input_index<input_length)
        {
            split_octet_2(input_data[input_index++], b2, b3);
            output_data[output_index++] = base64_chars[b2];

            if(input_index<input_length)
            {
                split_octet_3(input_data[input_index++], b3, b4);
                output_data[output_index++] = base64_chars[b3];
                output_data[output_index++] = base64_chars[b4];
            }
            else
            {
                output_data[output_index++] = base64_chars[b3];
                output_data[output_index++] = '=';
            }
        }
        else
        {
            output_data[output_index++] = base64_chars[b2];
            output_data[output_index++] = '=';
            output_data[output_index++] = '=';
        }

        output_data[output_index++] = '\n';
        output_data[output_index] = 0;

        *output_length = output_index;

        return output_data;
    }

    static inline char set_octet_1(char b1, char b2)
    {
        return (( b1 << 2 ) | ( b2 >> 4 ));
    }

    static inline char set_octet_2(char b2, char b3)
    {
        return (( b2 << 4 ) | ( b3 >> 2 ));
    }

    static inline char set_octet_3(char b3, char b4)
    {
        return (( b3 << 6 ) | b4 );
    }

    char* b64_decode(const char* input_data, unsigned int input_length, unsigned int* output_length)
    {
        char* raw_input_data = (char*)malloc(input_length);
        boost::scoped_ptr<char> ptr(raw_input_data);

        unsigned int input_index = 0;
        unsigned int raw_input_length = 0;
        while (input_index < input_length)
        {
            raw_input_data[raw_input_length] = input_data[input_index++];

            if (isspace(input_data[input_index])==0)
                ++raw_input_length;
        }

        if ((raw_input_length % 4) != 0 )
            return 0;

        int quad_count = (int)raw_input_length / 4;
        if ( quad_count == 0 )
            return 0;

        char  b1, b2, b3, b4;

        unsigned int raw_input_index = 0;
        unsigned int output_index = 0;
        char *output_data = (char*)malloc(quad_count*3+1);

        for (int quad=1; quad<quad_count; ++quad)
        {
            b1 = base64_inverse[input_data[raw_input_index++]];
            b2 = base64_inverse[input_data[raw_input_index++]];
            b3 = base64_inverse[input_data[raw_input_index++]];
            b4 = base64_inverse[input_data[raw_input_index++]];

            if (b1==(char)0xFF || b2==(char)0xFF || b3==(char)0xFF || b4==(char)0xFF)
            {
                free(output_data);
                return 0;
            }

            output_data[output_index++] = set_octet_1(b1, b2);
            output_data[output_index++] = set_octet_2(b2, b3);
            output_data[output_index++] = set_octet_3(b3, b4);
        }

        b1 = base64_inverse[input_data[raw_input_index++]];
        b2 = base64_inverse[input_data[raw_input_index++]];

        if (b1==(char)0xFF || b2==(char)0xFF)
        {
            free(output_data);
            return 0;
        }

        // try to process last two octets
        char d3 = input_data[raw_input_index++];
        char d4 = input_data[raw_input_index++];

        b3 = base64_inverse[d3];
        b4 = base64_inverse[d4];
       
        if (b3==(char)0xFF || b4==(char)0xFF)
        {
            if (d3=='=' && d4=='=')
            {
                output_data[output_index++] = set_octet_1(b1, b2);
            }
            else if (d3!='=' && d4=='=')
            {
                output_data[output_index++] = set_octet_1(b1, b2);
                output_data[output_index++] = set_octet_2(b2, b3);
            }
            else
            {
                free(output_data);
                return 0;
            }
        }
        else
        {
            output_data[output_index++] = set_octet_1(b1, b2);
            output_data[output_index++] = set_octet_2(b2, b3);
            output_data[output_index++] = set_octet_3(b3, b4);
        }

        output_data[output_index] = 0;
        *output_length = output_index;

        return output_data;
    }

}} // protean::detail
