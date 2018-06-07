using System;
using System.Collections.Generic;

namespace Protean {

    // Primitives
    public abstract class VariantPrimitiveBase
    {
        static readonly Dictionary<Type, VariantBase.EnumType> m_typeToEnumMapping =
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

        static readonly Dictionary<VariantBase.EnumType, Type> m_enumToTypeMapping =
            new Dictionary<VariantBase.EnumType, Type> {
		        { VariantBase.EnumType.String, typeof(string) },
		        { VariantBase.EnumType.Boolean, typeof(bool) },
		        { VariantBase.EnumType.Int32, typeof(Int32) },
		        { VariantBase.EnumType.UInt32, typeof(UInt32) },
		        { VariantBase.EnumType.Int64, typeof(Int64) },
		        { VariantBase.EnumType.UInt64, typeof(UInt64) },
		        { VariantBase.EnumType.Float, typeof(float) },
		        { VariantBase.EnumType.Double, typeof(double) },
                { VariantBase.EnumType.DateTime, typeof(DateTime) },
                { VariantBase.EnumType.Time, typeof(TimeSpan) }
            };


        public static string ToString<T>(T value)
        {
            var type = typeof(T);

            if (type == typeof(TimeSpan))
            {
                return VariantBase.ToString((TimeSpan)Convert.ChangeType(value, type));
            }
            else
            {
                if (type == typeof(double))
                {
                    return VariantBase.ToString((double)Convert.ChangeType(value, type));
                }
                else if (type == typeof(bool))
                {
                    return VariantBase.ToString((bool)Convert.ChangeType(value, type));
                }
                else if (type == typeof(DateTime))
                {
                    return VariantBase.ToString((DateTime)Convert.ChangeType(value, type));
                }
                else
                {
                    return value.ToString();
                }
            }
        }

        public static T Parse<T>(string value)
        {
            var type = typeof(T);

            if (type == typeof(TimeSpan))
            {
                return (T)Convert.ChangeType(VariantBase.ParseTime(value), type);
            }
            else
            {
                if (type == typeof(float))
                {
                    return (T)Convert.ChangeType(VariantBase.ParseSingle(value), type);
                }
                else if (type == typeof(double))
                {
                    return (T)Convert.ChangeType(VariantBase.ParseDouble(value), type);
                }
                else if (type == typeof(bool))
                {
                    return (T)Convert.ChangeType(VariantBase.ParseBoolean(value), type);
                }
                else if (type == typeof(DateTime))
                {
                    return (T)Convert.ChangeType(VariantBase.ParseDateTime(value), type);
                }
                else
                {
                    return (T)Convert.ChangeType(value, type);
                }
            }
        }

		public static VariantBase.EnumType TypeToEnum(Type type)
        {
			VariantBase.EnumType enumType;

			if (!m_typeToEnumMapping.TryGetValue(type, out enumType))
			{
				throw new VariantException("Case exhaustion: " + type.Name);
			}

			return enumType;
        }

        public static Type EnumToType(Variant.EnumType type)
        {
        	Type result;

            if (!m_enumToTypeMapping.TryGetValue(type, out result))
            {
                throw new VariantException("Case exhaustion: " + type);
            }

        	return result;
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

        public override int GetHashCode()
        {
            return EqualityComparer<T>.Default.GetHashCode(Value);
        }

        public T Value { get; set; }
    }

} // Protean
