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
        IConvertible,
        IEquatable<Variant>
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

        public Variant(EnumType type, int size) :
            base(type, size)
        { }

        public Variant(string arg) {
            Value = new VariantPrimitive<String>(arg);
        }

        public Variant(bool arg) {
            Value = new VariantPrimitive<bool>(arg);
        }
        public Variant(Int32 arg) {
            Value = new VariantPrimitive<Int32>(arg);
        }

        public Variant(UInt32 arg) {
            Value = new VariantPrimitive<UInt32>(arg);
        }

        public Variant(Int64 arg) {
            Value = new VariantPrimitive<Int64>(arg);
        }

        public Variant(UInt64 arg) {
            Value = new VariantPrimitive<UInt64>(arg);
        }

        public Variant(double arg) {
            Value = new VariantPrimitive<double>(arg);
        }

        public Variant(TimeSpan arg) {
            Value = new VariantPrimitive<TimeSpan>(arg);
        }

        public Variant(DateTime arg) {
            Value = new VariantPrimitive<DateTime>(arg);
        }

        public Variant(byte[] arg) {
            Value = new VariantBuffer(arg);
        }

        public Variant(VariantObjectBase arg)  {
            Value = arg;
        }

        public Variant(System.Exception arg) :
            this(new ExceptionInfo(arg))
        {  }

        public  Variant(ExceptionInfo arg) {
            Value = arg;
        }

        public VariantObjectBase AsObject()
        {
            CheckType(EnumType.Object, "AsObject()");

            return Value as VariantObjectBase;
        }

        public T AsObject<T>()
            where T : VariantObjectBase, new()
        {
            CheckType(EnumType.Object, "AsObject()");

            VariantObjectBase o = Value as VariantObjectBase;

            T result = o as T;

            if (result==null)
            {
                result = new T();
                result.Coerce(o);

                if (o is VariantObjectProxy)
                {
                    Value = result;
                }
            }

            return result;
        }

        public ExceptionInfo AsException()
        {
            CheckType(EnumType.Exception, "AsException()");

            return Value as ExceptionInfo;
        }

        // Lists
        public void Add(Variant value)
        {
            CheckType(EnumType.List, "Add");

            (Value as VariantList).Value.Add(value);
        }

        // Bags/Dictionaries
        public bool ContainsKey(string key)
        {
            CheckType(EnumType.Mapping, "ContainsKey");

            return (Value as IVariantMapping).ContainsKey(key);
        }

        public void Add(string key, Variant value)
        {
            CheckType(EnumType.Mapping, "Add");

            (Value as IVariantMapping).Add(key, value);
        }

        public void Remove(string key)
        {
            CheckType(EnumType.Mapping, "Remove");

            (Value as IVariantMapping).Remove(key);
        }

        public Variant Range(string key)
        {
            CheckType(EnumType.Mapping, "Range");

            return (Value as IVariantMapping).Range(key);
        }

        // TimeSeries
        public void Add(DateTime time, Variant value)
        {
            CheckType(EnumType.TimeSeries, "Add");

            (Value as VariantTimeSeries).Value.Add(new KeyValuePair<DateTime, Variant>(time, value));
        }

        public int Count
        {
            get {
                CheckType(EnumType.Collection, "Count"); 

                return (Value as IVariantCollection).Count;
            }
        }

        public bool Empty
        {
            get { return Count==0; }
        }

        public Variant this[string key]
        {
            get {
                CheckType(EnumType.Mapping, "this[]");

                return (Value as IVariantMapping)[key];
            }
            set {
                CheckType(EnumType.Mapping, "this[]");

                (Value as IVariantMapping)[key] = value;
            }
        }

        public Variant this[int index]
        {
            
            get {
                CheckType(EnumType.Sequence, "this[]");

                return (Value as IVariantSequence)[index];
            }

            set  {
                CheckType(EnumType.Sequence, "this[]");

                (Value as IVariantSequence)[index] = value;
            }
        }

        public Variant As(EnumType type)
        {
            return new Variant();
        }

        string ToString(bool summarise, string indent)
        {
            const string tab = "   ";
            const string no_indent = "";

            string result = indent;
        
            switch (Type)
            {
                case EnumType.None:
                    result += "None";
                    break;
                case EnumType.Any:
                    result += "Any('" + As<string>() + "')";
                    break;
                case EnumType.String:
                    result += "'" + As<string>() + "'";
                    break;
                case EnumType.Int32:
                case EnumType.UInt32:
                case EnumType.Int64:
                case EnumType.UInt64:
                case EnumType.Float:
                case EnumType.Double:
                case EnumType.Boolean:
                case EnumType.Time:
                case EnumType.DateTime:
                    result += "value";
                    //result += any_cast().as<std::string>();
                    break;
                case EnumType.List:
                {
                    if (summarise)
                    {
                        result += "List(size=" + Count.ToString() + ")";
                    }
                    else
                    {
                        result += "[\n";
                        int count = Count;
                        foreach (VariantItem item in this)
                        {
                            result += item.Value.ToString(false, indent+tab);
                            if (--count>0)
                            {
                                result += ",";
                            }
                            result += "\n";
                        }
                        result += indent + "]";
                    }
                    break;
                }
                case EnumType.Dictionary:
                {
                    if (summarise)
                    {
                        result += "Dictionary(size=" + Count.ToString() + ")";
                    }
                    else
                    {
                        result += "{\n";
                        int count = Count;
                        foreach (VariantItem item in this)
                        {
                            result += indent + tab + item.Key + ": ";
                            if (item.Value.Is(EnumType.Primitive))
                            {
                                result += item.Value.ToString(false, no_indent);
                            }
                            else
                            {
                                result += "\n" + item.Value.ToString(false, indent + tab + tab);
                            }
                            if (--count!=0)
                            {
                                result += ",";
                            }
                            result += "\n";
                        }
                        result += indent + "}";
                    }
                    break;
                }
                case EnumType.Bag:
                {
                    if (summarise)
                    {
                        result += "Bag(size=" + Count + ")";
                    }
                    else
                    {
                        result += "[\n";
                        int count = Count;
                        foreach (VariantItem item in this)
                        {
                            result += indent + tab + "(" + item.Key + ", ";
                            if (item.Value.Is(EnumType.Primitive))
                            {
                                result += item.Value.ToString(false, no_indent);
                            }
                            else
                            {
                                result += "\n" + item.Value.ToString(false, indent+tab);
                            }
                            result += ")";
                            if (--count!=0)
                            {
                                result += ",";
                            }
                            result += "\n";
                        }
                        result += indent + "]";
                    }
                    break;
                }
                case EnumType.TimeSeries:
                {
                    if (summarise)
                    {
                        result += "TimeSeries(size=" + Count + ")";
                    }
                    else
                    {
                        result += "TimeSeries(\n";
                        int count = Count;
                        foreach (VariantItem item in this)
                        {
                            result += indent + tab + "(" + item.Time + ", ";
                            if (item.Value.Is(EnumType.Primitive))
                            {
                                result += item.Value.ToString(false, no_indent);
                            }
                            else
                            {
                                result += "\n" + item.Value.ToString(false, indent + tab);
                            }
                            result += ")";
                            if (--count!=0)
                            {
                                result += ",";
                            }
                            result += "\n";
                        }
                        result += indent + ")";
                    }
                    break;
                }
                case EnumType.Buffer:
                {
                    if (summarise)
                    {
                        result += "Buffer(size=" + Count + ")";
                    }
                    else
                    {
                        result += "Buffer(";

                        byte[] bytes = (Value as VariantBuffer).Value;
                        result += bytes.ToString();
                        result += ')';
                    }
                    break;
                }
                case EnumType.Tuple:
                {
                    if (summarise)
                    {
                        result += "Tuple(size=" + Count + ")";
                    }
                    else
                    {
                        result += "(\n";
                        int count = Count;
                        foreach (VariantItem item in this)
                        {
                            result += item.Value.ToString(false, indent+tab);
                            if (--count!=0)
                            {
                                result += ",";
                            }
                            result += "\n";

                        }
                        result += indent + ")";
                    }
                    break;
                }
                case EnumType.Exception:
                {
                    ExceptionInfo x = AsException();

                    result += x.Class + "('" + x.Message + "')";
                    
                    if (x.Source.Length>0)
                    {
                        result += " in: " + x.Source;
                    }

                    if (x.Stack.Length>0)
                    {
                        result += "\n" + x.Stack;
                    }
                    break;
                }
                case EnumType.Object:
                {
                    VariantObjectBase o = AsObject();

                    if (summarise)
                    {
                        result += o.Class + "(version=" + o.Version + ")";
                    }
                    else
                    {
                        Variant param = o.Deflate();
                        
                        result += o.Class + "(version=" + o.Version + ")(\n";
                        result += param.ToString(false, indent + tab);
                        result += "\n" + indent + ")";
                    }
                    break;
                }
                default:
                    result += "UNKNOWN<" + Type + ">";
                    break;
            }
            return result;
        }

        public override String ToString()
        {
            return ToString(false, "");
        }

        String ToString(bool summarise)
        {
            return ToString(summarise, "");
        }

        public IEnumerator<VariantItem> GetEnumerator()
        {
            CheckType(EnumType.Collection, "GetEnumerator()");

            return (Value as IVariantCollection).GetEnumerator();
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

        // System.IEquatable
        public bool Equals(Variant obj)
        {
            // TODO!
            throw new VariantException("TODO: Implement IEquatable");
        }

        // helpers
        private void CheckType(EnumType type, string fn)
        {
            if (!Is(type))
            {
                throw new VariantException(string.Format("Calling {0} on Variant of type {1}\n{2}", fn, type, ToString()));
            }
        }

        public static readonly DateTime MinDateTime = new DateTime(1400, 1, 1);
        public static readonly DateTime MaxDateTime = new DateTime(9999, 12, 31, 23, 59, 59, 999);
    };


} // protean
