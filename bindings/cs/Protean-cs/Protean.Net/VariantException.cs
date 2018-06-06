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
