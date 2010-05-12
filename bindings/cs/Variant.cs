//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Text;

namespace protean {

    public class Variant :
        VariantBase,
        IEnumerable<VariantItem>,
        IConvertible
    {
        
        public Variant()
        { } 

        public Variant(Variant arg)
        {
            Type = arg.Type;
            Assign(arg.Type, arg);
        }

        public Variant(EnumType type)
        {
            Type = type;
            Initialise(type, 0);
        }

        public Variant(EnumType type, UInt32 size)
        {
            Type = type;
            Initialise(type, size);
        }

        public Variant(String arg)
        {
            Type = EnumType.String;
            base.String = arg;
        }

        public Variant(bool arg)
        {
            Type = EnumType.Boolean;
            base.Boolean = arg;
        }

        public Variant(Int32 arg)
        {
            Type = EnumType.Int32;
            base.Int32 = arg;
        }

        public Variant(UInt32 arg)
        {
            Type = EnumType.UInt32;
            base.UInt32 = arg;
        }

        public Variant(Int64 arg)
        {
            Type = EnumType.Int64;
            base.Int64 = arg;
        }

        public Variant(UInt64 arg)
        {
            Type = EnumType.UInt64;
            base.UInt64 = arg;
        }

        public Variant(float arg)
        {
            Type = EnumType.Float;
            base.Float = arg;
        }

        public Variant(double arg)
        {
            Type = EnumType.Double;
            base.Double = arg;
        }

        public Variant(System.TimeSpan arg)
        {
            Type = EnumType.Time;
            base.Time = arg;
        }

        public Variant(System.DateTime arg) :
            this(arg, false)
        {
        }

        public Variant(System.DateTime arg, bool ignoreTime)
        {
            Type = (ignoreTime ? EnumType.Time : EnumType.DateTime);
            base.DateTime = arg;
        }

        public Variant(byte[] arg)
        {
            Type = EnumType.Buffer;
            base.Buffer = arg;
        }

        public Variant(IVariantObject arg)
        {
            Type = EnumType.Object;
            base.Object = arg;
        }

        public Variant(System.Exception arg) :
            this(new ExceptionInfo(arg))
        {
        }

        public  Variant(ExceptionInfo arg)
        {
            Type = EnumType.Exception;
            base.Exception = arg;
        }

        /**
         * Type inspection methods
         */
        public bool Is(EnumType type)
        {
            return (int)(Type & type)!=0;
        }

        /**
         * Type query methods
         */
        public new String String
        {
            get
            {
                return base.String;
            }
        }
        public new bool Boolean
        {
            get
            {
                return base.Boolean;
            }
        }
        public new Int32 Int32
        {
            get
            {
                return base.Int32;
            }
        }
        public new UInt32 UInt32
        {
            get
            {
                return base.UInt32;
            }
        }
        public new Int64 Int64
        {
            get
            {
                return base.Int64;
            }
        }
        public new UInt64 UInt64
        {
            get
            {
                return base.UInt64;
            }
        }
        public new float Float
        {
            get
            {
                return base.Float;
            }
        }
        public new double Double
        {
            get
            {
                return base.Double;
            }
        }
        public new DateTime Date
        {
            get
            {
                return base.Date;
            }
        }
        public new DateTime DateTime
        {
            get
            {
                return base.DateTime;
            }
        }
        public new TimeSpan Time
        {
            get
            {
                return base.Time;
            }
        }
        public new byte[] Buffer
        {
            get
            {
                return base.Buffer;
            }
        }

        public T AsObject<T>()
            where T : IVariantObject, new()
        {
            T newObj = new T();

            return newObj;
        }

        public new IVariantObject Object
        {
            get
            {
                return base.Object;
            }
        }

        public new ExceptionInfo Exception
        {
            get
            {
                return base.Exception;
            }
        }

        // Lists
        public void Add(Variant value)
        {
            base.List.Add(value);
        }

        // Bags/Dictionaries
        public bool ContainsKey(String key)
        {
            return base.Dictionary.ContainsKey(key);
        }

        public void Add(String key, Variant value)
        {
            base.Dictionary.Add(key, value);
        }

        public void Remove(String key)
        {
            base.Dictionary.Remove(key);
        }

        // TimeSeries
        public void Add(DateTime time, Variant value)
        {
            base.TimeSeries.Add(new KeyValuePair<DateTime, Variant>(time, value));
        }

        public int Count
        {
            get { return base.Collection.Count; }
        }

        public bool Empty
        {
            get { return Count==0; }
        }

        public Variant this[String key]
        {
            get { return base.Dictionary[key]; }
            set { base.Dictionary[key] = value; }
        }

        public Variant this[UInt32 index]
        {
            get
            {
                if (Type==EnumType.Tuple)
                {
                    return base.Tuple[index];
                }
                else
                {
                    return base.List[(int)index];
                }
            }

            set
            {
                if (Type==EnumType.Tuple)
                {
                    base.Tuple[index] = value;
                }
                else
                {
                    base.List[(int)index] = value;
                }
            }
        }

        public override String ToString()
        {
            // TODO
            return ToString(false);
        }

        String ToString(bool summarise)
        {
            // TODO
            return Type.ToString();
        }

        public IEnumerator<VariantItem> GetEnumerator()
        {
            return new VariantEnumerator(Type, base.Enumerable);
        }


        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }


        // System::IConvertible
        public TypeCode GetTypeCode()
        {
            return TypeCode.Object;
        }
        public bool ToBoolean(IFormatProvider provider)
        {
            return Boolean;
        }
        public byte ToByte(IFormatProvider provider)
        {
            throw new VariantException("Variant does not support ToByte");
        }
        public char ToChar(IFormatProvider provider)
        {
            throw new VariantException("Variant does not support ToChar");
        }
        public DateTime ToDateTime(IFormatProvider provider)
        {
            return DateTime;
        }
        public decimal  ToDecimal(IFormatProvider provider)
        {
            throw new VariantException("Variant does not support ToDecimal");
        }
        public double ToDouble(IFormatProvider provider)
        {
            return Double;
        }
        public short ToInt16(IFormatProvider provider)
        {
            throw new VariantException("Variant does not support ToInt16");
        }
        public int ToInt32(IFormatProvider provider)
        {
            return Int32;
        }
        public long ToInt64(IFormatProvider provider)
        {
            return Int64;
        }
        public sbyte  ToSByte(IFormatProvider provider)
        {
            throw new VariantException("Variant does not support ToSByte");
        }
        public float ToSingle(IFormatProvider provider)
        {
            return Float;
        }
        public String ToString(IFormatProvider provider)
        {
            return String;
        }
        public Object ToType(Type conversionType, IFormatProvider provider)
        {
            Type[] types = new Type[] { typeof(Variant) };

            System.Reflection.MethodInfo conversionMethod = conversionType.GetMethod("op_Implicit", types);

            if (conversionMethod==null)
            {
                return null;
            }

            Object[] args = new Object[] { this };

            return conversionMethod.Invoke(null, args);
        }
        public ushort ToUInt16(IFormatProvider provider)
        {
            throw new VariantException("Variant does not support ToUInt16");
        }
        public uint ToUInt32(IFormatProvider provider)
        {
            return UInt32;
        }
        public ulong ToUInt64(IFormatProvider provider)
        {
            return UInt64;
        }

        public EnumType Type { get; private set; }

    };


} // protean
