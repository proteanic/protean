//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Text;

namespace protean {

    public abstract class VariantObjectBase : IVariantData
    {
        public abstract string Class { get; }

        public abstract int Version { get; }

        public abstract Variant Deflate();

        public abstract void Inflate(Variant param, int version);

        public void Coerce(VariantObjectBase rhs)
        {
            if (Class!=rhs.Class)
            {
                throw new VariantException(string.Format("Attempt to coerce object of type {0} into {1}", rhs.Class, Class));
            }

            Variant param = rhs.Deflate();
            Inflate(param, rhs.Version);
        }

        public VariantBase.EnumType Type
        {
            get { return VariantBase.EnumType.Object; }
        }

        public int CompareTo(IVariantData rhs)
        {
            VariantObjectBase rhsObj = (VariantObjectBase)rhs;
            if (Class != rhsObj.Class)
            {
                return Class.CompareTo(rhsObj.Class);
            }
            if (Version != rhsObj.Version)
            {
                return Version.CompareTo(rhsObj.Version);
            }
            return Deflate().CompareTo(rhsObj.Deflate());
        }
    };

} // protean
