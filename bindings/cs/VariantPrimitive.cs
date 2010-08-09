//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Text;

namespace protean {

    // Primitives
    internal abstract class VariantPrimitiveBase
    {
        static Dictionary<Type, VariantBase.EnumType> m_typeMapping =
            new Dictionary<Type, VariantBase.EnumType> {
		        { typeof(string), VariantBase.EnumType.String },
		        { typeof(bool), VariantBase.EnumType.Boolean },
		        { typeof(Int32), VariantBase.EnumType.Int32 },
		        { typeof(UInt32), VariantBase.EnumType.UInt32 },
		        { typeof(Int64), VariantBase.EnumType.Int64 },
		        { typeof(UInt64), VariantBase.EnumType.UInt64 },
		        { typeof(float), VariantBase.EnumType.Float },
		        { typeof(double), VariantBase.EnumType.Double },
                { typeof(DateTime), VariantBase.EnumType.DateTime },
                { typeof(TimeSpan), VariantBase.EnumType.Time }
            };

        public static string ToString<T>(T value)
        {
            if (typeof(T) == typeof(TimeSpan))
            {
                return VariantBase.ToString((TimeSpan)Convert.ChangeType(value, typeof(TimeSpan)));
            }
            else
            {
                TypeCode typeCode = System.Type.GetTypeCode(typeof(T));
                switch (typeCode)
                {
                    case TypeCode.Double:
                        return VariantBase.ToString((double)Convert.ChangeType(value, typeof(double)));
                    case TypeCode.Boolean:
                        return VariantBase.ToString((bool)Convert.ChangeType(value, typeof(bool)));
                    case TypeCode.DateTime:
                        return VariantBase.ToString((DateTime)Convert.ChangeType(value, typeof(DateTime)));
                    default:
                        return value.ToString();
                }
            }
        }

        public static T Parse<T>(string value)
        {
            if (typeof(T) == typeof(TimeSpan))
            {
                return (T)Convert.ChangeType(VariantBase.ParseTime(value), typeof(T));
            }
            else
            {

                TypeCode typeCode = System.Type.GetTypeCode(typeof(T));
                switch (typeCode)
                {
                    case TypeCode.Double:
                        return (T)Convert.ChangeType(VariantBase.ParseDouble(value), typeof(T));
                    case TypeCode.Boolean:
                        return (T)Convert.ChangeType(VariantBase.ParseBoolean(value), typeof(T));
                    case TypeCode.DateTime:
                        return (T)Convert.ChangeType(VariantBase.ParseDateTime(value), typeof(T));
                    default:
                        return (T)Convert.ChangeType(value, typeof(T));
                }
            }
        }

        public static VariantBase.EnumType TypeToEnum(Type type)
        {
            if (m_typeMapping.ContainsKey(type))
            {
                return m_typeMapping[type];
            }
            else
            {
                throw new VariantException("Case exhaustion: " + type.Name);
            }
        }
    }

    internal class VariantPrimitive<T> :
        VariantPrimitiveBase,
        IVariantData
        where T : IComparable<T>
    {
        public VariantPrimitive(T value) {
            Value = value;
        }

        public VariantPrimitive(VariantPrimitive<T> rhs) :
            this(rhs.Value)
        {  }

        public static implicit operator VariantPrimitive<T>(T value) {
            return new VariantPrimitive<T>(value);
        }

        public VariantBase.EnumType _Type  {
            get { return TypeToEnum(Value.GetType());  }
        }

        public override string ToString()
        {
            return VariantPrimitiveBase.ToString<T>(Value);
        }

        public int CompareTo(IVariantData rhs)
        {
            return Value.CompareTo(((VariantPrimitive<T>)rhs).Value);
        }

        public T Value { get; set; }
    }

} // protean
