//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;

namespace Protean
{
    public sealed class BinaryConstants
    {
        public const UInt32 PROTEAN_MAGIC = 0x484913FF;
        public const UInt32 VERSION_MAJOR = 1;
        public const UInt32 VERSION_MINOR = 0;

        public const byte ZLIB_CMF = 0x58;
        public const byte ZLIB_FLG = 0x85;
    }
}
