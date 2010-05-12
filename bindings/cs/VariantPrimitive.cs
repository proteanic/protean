//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Text;

namespace protean {

    // Primitives
    public interface IVariantPrimitive : IVariantData
    { }

    public class VariantPrimitive<T> : IVariantPrimitive
    {
        public VariantPrimitive(T value)
        {
            Value = value;
        }

        public VariantPrimitive(VariantPrimitive<T> rhs)
        {
            Value = rhs.Value;
        }

        public static implicit operator VariantPrimitive<T>(T value)
        {
            return new VariantPrimitive<T>(value);
        }

        public VariantBase.EnumType Type
        {
            get
            {
                if (Value.GetType() == typeof(String))
                {
                    return VariantBase.EnumType.String;
                }
                if (Value.GetType() == typeof(bool))
                {
                    return VariantBase.EnumType.Boolean;
                }
                else if (Value.GetType() == typeof(Int32))
                {
                    return VariantBase.EnumType.Int32;
                }
                else if (Value.GetType() == typeof(UInt32))
                {
                    return VariantBase.EnumType.UInt32;
                }
                else if (Value.GetType() == typeof(Int64))
                {
                    return VariantBase.EnumType.Int64;
                }
                else if (Value.GetType() == typeof(UInt64))
                {
                    return VariantBase.EnumType.UInt64;
                }
                else if (Value.GetType() == typeof(float))
                {
                    return VariantBase.EnumType.Float;
                }
                else if (Value.GetType() == typeof(double))
                {
                    return VariantBase.EnumType.Double;
                }
                else if (Value.GetType() == typeof(DateTime))
                {
                    return VariantBase.EnumType.DateTime;
                }
                else if (Value.GetType() == typeof(TimeSpan))
                {
                    return VariantBase.EnumType.Time;
                }
                else
                {
                    throw new VariantException("Case exhaustion: " + Value.GetType().Name);
                }
            }
        }

        public T Value { get; set; }
    }

} // protean
