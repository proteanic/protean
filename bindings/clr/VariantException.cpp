//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include "VariantException.hpp"

namespace protean { namespace clr {

    VariantException::VariantException(System::String^ message)
        : System::ApplicationException(message)
    {
    }

}} // protean::clr
