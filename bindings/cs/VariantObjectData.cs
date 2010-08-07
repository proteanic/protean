//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Text;

namespace protean {

    internal class VariantObjectData : IVariantData
    {
        public VariantObjectData(IVariantObject obj)
        {
            Value = obj;
        }

        public VariantBase.EnumType _Type
        {
            get { return VariantBase.EnumType.Object; }
        }

        public int CompareTo(IVariantData rhs)
        {
            IVariantObject rhsObj = ((VariantObjectData)rhs).Value;
            if (Value.Class != rhsObj.Class)
            {
                return Value.Class.CompareTo(rhsObj.Class);
            }
            if (Value.Version != rhsObj.Version)
            {
                return Value.Version.CompareTo(rhsObj.Version);
            }
            return Value.Deflate().CompareTo(rhsObj.Deflate());
        }

        public IVariantObject Value { get; private set; }
    };

} // protean
