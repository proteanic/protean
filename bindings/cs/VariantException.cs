//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Runtime.Serialization;

namespace Protean {

    [Serializable]
    public class VariantException :
        System.ApplicationException
    {
        public VariantException(String message) :
            base(message)
        { }

        protected VariantException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        { }
    }

} // Protean
