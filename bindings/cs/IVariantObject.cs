//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

namespace Protean {

    public interface IVariantObject
    {
        string Class { get; }

        int Version { get; }

        Variant Deflate();

        void Inflate(Variant param, int version);
    };

} // Protean
