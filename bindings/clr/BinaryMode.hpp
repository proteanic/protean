//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CLR_BINARY_MODE_HPP
#define PROTEAN_CLR_BINARY_MODE_HPP

#include <protean/binary_common.hpp>

namespace protean { namespace clr {

    public enum class BinaryMode
    {
        None            = protean::binary_mode::None,
        Compress        = protean::binary_mode::Compress,
        ZlibHeader      = protean::binary_mode::ZlibHeader,
        //CreateProxy     = protean::binary_mode::CreateProxy, // not supported in CLR yet
        DateTimeAsTicks = protean::binary_mode::DateTimeAsTicks,
        Default         = protean::binary_mode::None,
    };

}}

#endif // PROTEAN_CLR_BINARY_MODE_HPP
