//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
#if !DISABLE_SERIALIZATION
using System.Runtime.Serialization;
#endif

namespace Protean {

#if !DISABLE_SERIALIZATION
    [Serializable]
#endif
    public class VariantException : Exception
    {
        public VariantException(string message) :
            base(message)
        { }

#if !DISABLE_SERIALIZATION
        protected VariantException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        { }
#endif
    }

} // Protean
