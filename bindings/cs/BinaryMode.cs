//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Text;

namespace protean
{
    public enum BinaryMode
    {
        None            = 0x00000000,
        Compress        = 0x00000001,   // BinaryWriter: compress data
        ZlibHeader      = 0x00000002,   // BinaryWriter: output zlib header
        //CreateProxy     = 0x00000004,   // binary_reader: create proxy object if class has not been registered in factory
        DateTimeAsTicks = 0x00000008,   // binary_writer: serialise DateTime/Date/Time as milliseconds/days since 1/1/1400
        Default = DateTimeAsTicks
    }
}
