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
        public Variant() :
            base()
        { } 

        public Variant(Variant arg) :
            base(arg)
        { }

        public Variant(EnumType type) :
            base(type, 0)
        { }

        public Variant(EnumType type, UInt32 size) :
            base(type, size)
        { }

        public Variant(String arg)
        {
            Value = new VariantPrimitive<String>(arg);
        }

        public Variant(bool arg)
        {
            Value = new VariantPrimitive<bool>(arg);
        }
        public Variant(Int32 arg)
        {
            Value = new VariantPrimitive<Int32>(arg);
        }

        public Variant(UInt32 arg)
        {
            Value = new VariantPrimitive<UInt32>(arg);
        }

        public Variant(Int64 arg)
        {
            Value = new VariantPrimitive<Int64>(arg);
        }

        public Variant(UInt64 arg)
        {
            Value = new VariantPrimitive<UInt64>(arg);
        }

        public Variant(float arg)
        {
            Value = new VariantPrimitive<float>(arg);
        }

        public Variant(double arg)
        {
            Value = new VariantPrimitive<double>(arg);
        }

        public Variant(TimeSpan arg)
        {
            Value = new VariantPrimitive<TimeSpan>(arg);
        }

        public Variant(DateTime arg)
        {
            Value = new VariantPrimitive<DateTime>(arg);
        }

        public Variant(byte[] arg)
        {
            Value = new VariantBuffer(arg);
        }

        public Variant(VariantObjectBase arg)
        {
            Value = arg;
        }

        public Variant(System.Exception arg) :
            this(new ExceptionInfo(arg))
        {
        }

        public  Variant(ExceptionInfo arg)
        {
            Value = arg;
        }

        public T AsObject<T>()
            where T : VariantObjectBase, new()
        {
            T newObj = new T();

            return newObj;
        }

        // Lists
        public void Add(Variant value)
        {
            (Value as VariantList).Value.Add(value);
        }

        // Bags/Dictionaries
        public bool ContainsKey(String key)
        {
            return (Value as VariantDictionary).Value.ContainsKey(key);
        }

        public void Add(String key, Variant value)
        {
            (Value as VariantDictionary).Value.Add(key, value);
        }

        public void Remove(String key)
        {
            (Value as VariantDictionary).Value.Remove(key);
        }

        // TimeSeries
        public void Add(DateTime time, Variant value)
        {
            (Value as VariantTimeSeries).Value.Add(new KeyValuePair<DateTime, Variant>(time, value));
        }

        public int Count
        {
            get { return (Value as IVariantCollection).Count; }
        }

        public bool Empty
        {
            get { return Count==0; }
        }

        public Variant this[String key]
        {
            get { return (Value as VariantDictionary).Value[key]; }
            set { (Value as VariantDictionary).Value[key] = value; }
        }

        public Variant this[UInt32 index]
        {
            get
            {
                if (Value is VariantTuple)
                {
                    return (Value as VariantTuple).Value[(int)index];
                }
                else
                {
                    return (Value as VariantList).Value[(int)index];
                }
            }

            set
            {
                if (Value is VariantTuple)
                {
                    (Value as VariantTuple).Value[index] = value;
                }
                else
                {
                    (Value as VariantList).Value[(int)index] = value;
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
            if (Value is VariantList)
            {
                return new VariantEnumerator(EnumType.List, (Value as VariantList).Value);
            }
            else if (Value is VariantDictionary)
            {
                return new VariantEnumerator(EnumType.Dictionary, (Value as VariantDictionary).Value);
            }
            else if (Value is VariantBag)
            {
                return new VariantEnumerator(EnumType.Bag, (Value as VariantBag).Value);
            }
            else if (Value is VariantTimeSeries)
            {
                return new VariantEnumerator(EnumType.TimeSeries, (Value as VariantTimeSeries).Value);
            }
            else{
                throw new VariantException("Attempt to call GetEnumerator on " + Type.ToString() + "variant");
            }
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
            return As<bool>();
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
            return As<DateTime>();
        }
        public decimal  ToDecimal(IFormatProvider provider)
        {
            throw new VariantException("Variant does not support ToDecimal");
        }
        public double ToDouble(IFormatProvider provider)
        {
            return As<Double>();
        }
        public short ToInt16(IFormatProvider provider)
        {
            throw new VariantException("Variant does not support ToInt16");
        }
        public int ToInt32(IFormatProvider provider)
        {
            return As<Int32>();
        }
        public long ToInt64(IFormatProvider provider)
        {
            return As<Int64>();
        }
        public sbyte  ToSByte(IFormatProvider provider)
        {
            throw new VariantException("Variant does not support ToSByte");
        }
        public float ToSingle(IFormatProvider provider)
        {
            return As<float>();
        }
        public String ToString(IFormatProvider provider)
        {
            return As<String>();
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
            return As<UInt32>();
        }
        public ulong ToUInt64(IFormatProvider provider)
        {
            return As<UInt64>();
        }

        public static readonly DateTime MinDateTime = new DateTime(1400, 1, 1);
        public static readonly DateTime MaxDateTime = new DateTime(9999, 12, 31, 23, 59, 59, 999);
    };


} // protean
