//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Text;

namespace protean {

    public interface IVariantData :
        IComparable<IVariantData>
    {
        VariantBase.EnumType _Type { get; }
    }

    // None
    internal class VariantNone : IVariantData
    {
        public VariantNone()
        { }

        public VariantBase.EnumType _Type
        {
            get { return VariantBase.EnumType.None; }
        }

        public int CompareTo(IVariantData rhs)
        {
            return 0;
        }
    }

    // Any
    internal class VariantAny : IVariantData
    {
        public VariantAny() {
            Value = "";
        }

        public VariantAny(String value) {
            Value = value;
        }

        public VariantAny(VariantAny rhs) {
            Value = rhs.Value;
        }

        public VariantBase.EnumType _Type
        {
            get { return VariantBase.EnumType.Any; }
        }

        public int CompareTo(IVariantData rhs)
        {
            return Value.CompareTo(((VariantAny)rhs).Value);
        }

        public string Value { get; set; }
    }

    // Buffer
    internal class VariantBuffer : IVariantData
    {
        public VariantBuffer(int capacity) {
            Value = new byte[capacity];
        }

        public VariantBuffer(byte[] value) {
            Value = value;
        }

        public VariantBuffer(VariantBuffer rhs) {
            Value = new byte[rhs.Value.Length];
            Array.Copy(rhs.Value, Value, Value.Length);
        }

        public VariantBase.EnumType _Type
        {
            get { return VariantBase.EnumType.Buffer; }
        }

        public int CompareTo(IVariantData rhs)
        {
            return SequenceComparer.Compare(Value, ((VariantBuffer)rhs).Value);
        }

        public byte[] Value { get; set; }
    }

    public abstract class VariantBase
    {
        public enum EnumType
        {
            None                    = 0x00000001,
            Any                     = 0x00000002,
            String                  = 0x00000004,
            Boolean                 = 0x00000008,
            Int32                   = 0x00000010,
            UInt32                  = 0x00000020,
            Int64                   = 0x00000040,
            UInt64                  = 0x00000080,
            Float                   = 0x00000100,
            Double                  = 0x00000200,
            Date                    = 0x00000400,
            Time                    = 0x00000800,
            DateTime                = 0x00001000,
            List                    = 0x00002000,
            Dictionary              = 0x00004000,
            Bag                     = 0x00008000,
            Buffer                  = 0x00010000,
            Tuple                   = 0x00020000,
            Exception               = 0x00040000,
            TimeSeries              = 0x00080000,
            Object                  = 0x00100000,
            Array                   = 0x00200000,

            // The following enums represent groupings of variants
            Integer                 = Boolean | Int32 | UInt32 | Int64 | UInt64,
            Number                  = Integer | Float | Double,
            Primitive               = Number | Date | Time | DateTime | Any | String,
            Sequence                = List | Tuple,
            Mapping                 = Dictionary | Bag,
            Collection              = Sequence | Mapping | TimeSeries
        };

        internal IVariantData Value { get; set; }

        protected VariantBase()
        {
            Value = new VariantNone();
        }

        protected VariantBase(EnumType type, string value)
        {
            switch (type)
            {
                case EnumType.Any:
                    Value = new VariantAny(value);
                    break;
                case EnumType.String:
                    Value = new VariantPrimitive<String>(value);
                    break;
                case EnumType.Boolean:
                    Value = new VariantPrimitive<bool>(ParseBoolean(value));
                    break;
                case EnumType.Int32:
                    Value = new VariantPrimitive<Int32>(Int32.Parse(value));
                    break;
                case EnumType.UInt32:
                    Value = new VariantPrimitive<UInt32>(UInt32.Parse(value));
                    break;
                case EnumType.Int64:
                    Value = new VariantPrimitive<Int64>(Int64.Parse(value));
                    break;
                case EnumType.UInt64:
                    Value = new VariantPrimitive<UInt64>(UInt64.Parse(value));
                    break;
                case EnumType.Double:
                    Value = new VariantPrimitive<double>(double.Parse(value));
                    break;
                case EnumType.DateTime:
                    Value = new VariantPrimitive<DateTime>(ParseDateTime(value));
                    break;
                case EnumType.Time:
                    Value = new VariantPrimitive<TimeSpan>(ParseTime(value));
                    break;
                default:
                    throw new VariantException(string.Format("Unable to constract variant of type {0} from string", Type));
            }
        }

        protected VariantBase(EnumType type, int size)
        {
            switch (type)
            {
            case EnumType.None:
                Value = new VariantNone();
                break;
            case EnumType.Any:
                Value = new VariantAny();
                break;
            case EnumType.Boolean:
                Value = new VariantPrimitive<bool>(false);
                break;
            case EnumType.Int32:
                Value = new VariantPrimitive<Int32>(0);
                break;
            case EnumType.UInt32:
                Value = new VariantPrimitive<UInt32>(0);
                break;
            case EnumType.Int64:
                Value = new VariantPrimitive<Int64>(0);
                break;
            case EnumType.UInt64:
                Value = new VariantPrimitive<UInt64>(0);
                break;
            case EnumType.String:
                Value = new VariantPrimitive<String>("");
                break;
            case EnumType.DateTime:
                Value = new VariantPrimitive<DateTime>(new DateTime());
                break;
            case EnumType.Time:
                Value = new VariantPrimitive<TimeSpan>(new TimeSpan());
                break;
            case EnumType.List:
                Value = new VariantList(size);
                break;
            case EnumType.TimeSeries:
                Value = new VariantTimeSeries(size);
                break;
            case EnumType.Dictionary:
                Value = new VariantDictionary();
                break;
            case EnumType.Bag:
                Value = new VariantBag();
                break;
            case EnumType.Tuple:
                Value = new VariantTuple(size);
                break;
            case EnumType.Buffer:
                Value = new VariantBuffer(size);
                break;
            default:
                throw new VariantException("Cannot default construct variant of type: " + type.ToString());
            }
        }

        protected VariantBase(VariantBase rhs)
        {
            EnumType type = rhs.Type;
            switch (type)
            {
                case EnumType.None:
                    Value = new VariantNone();
                    break;
                case EnumType.Boolean:
                    Value = new VariantPrimitive<bool>(rhs.Value as VariantPrimitive<bool>);
                    break;
                case EnumType.Double:
                    Value = new VariantPrimitive<double>(rhs.Value as VariantPrimitive<double>);
                    break;
                case EnumType.Int32:
                    Value = new VariantPrimitive<Int32>(rhs.Value as VariantPrimitive<Int32>);
                    break;
                case EnumType.UInt32:
                    Value = new VariantPrimitive<UInt32>(rhs.Value as VariantPrimitive<UInt32>);
                    break;
                case EnumType.Int64:
                    Value = new VariantPrimitive<Int64>(rhs.Value as VariantPrimitive<Int64>);
                    break;
                case EnumType.UInt64:
                    Value = new VariantPrimitive<UInt64>(rhs.Value as VariantPrimitive<UInt64>);
                    break;
                case EnumType.DateTime:
                    Value = new VariantPrimitive<DateTime>(rhs.Value as VariantPrimitive<DateTime>);
                    break;
                case EnumType.Time:
                    Value = new VariantPrimitive<TimeSpan>(rhs.Value as VariantPrimitive<TimeSpan>);
                    break;
                case EnumType.String:
                    Value = new VariantPrimitive<String>(rhs.Value as VariantPrimitive<String>);
                    break;
                case EnumType.Any:
                    Value = new VariantAny(rhs.Value as VariantAny);
                    break;
                case EnumType.List:
                    Value = new VariantList(rhs.Value as VariantList);
                    break;
                case EnumType.Dictionary:
                    Value = new VariantDictionary(rhs.Value as VariantDictionary);
                    break;
                case EnumType.Bag:
                    Value = new VariantBag(rhs.Value as VariantBag);
                    break;
                case EnumType.TimeSeries:
                    Value = new VariantTimeSeries(rhs.Value as VariantTimeSeries);
                    break;
                case EnumType.Tuple:
                    Value = new VariantTuple(rhs.Value as VariantTuple);
                    break;
                case EnumType.Buffer:
                    Value = new VariantBuffer(rhs.Value as VariantBuffer);
                    break;
                case EnumType.Exception:
                    Value = new VariantExceptionInfo(rhs.Value as VariantExceptionInfo);
                    break;
                default:
                    throw new VariantException("Cannot copy variant of type: " + type.ToString());
            }
        }

        public EnumType Type
        {
            get
            {
                return Value._Type;
            }
        }

        // Is/As only work for primitive types
        public T As<T>() where T : IComparable<T>
        {
            if (Value is VariantPrimitive<T>)
            {
                return ((VariantPrimitive<T>)Value).Value;
            } 
            else if (Value is VariantAny)
            {
                string str = ((VariantAny)Value).Value;
                return VariantPrimitiveBase.Parse<T>(str);
            }
            else
            {
                throw new VariantException("Attempt to fetch " + typeof(T).Name + " from " + Type.ToString() + " variant.");
            }
        }

        public bool Is<T>() where T : IComparable<T>
        {
            return (Value is VariantPrimitive<T>);
        }

        public bool Is(EnumType type)
        {
            return (type & Value._Type)!=0;
        }

        public static double ParseDouble(string str)
        {
            if (str == "INF")
            {
                return double.PositiveInfinity;
            }
            else if (str == "-INF")
            {
                return double.NegativeInfinity;
            }
            else
            {
                return double.Parse(str);
            }
        }

        public static bool ParseBoolean(string str)
        {
            switch (str)
            {
                case "true":
                case "1":
                    return true;
                case "false":
                case "0":
                    return false;
                default:
                    throw new VariantException("Illegal format for Boolean, expecting true/1 or false/0, encountered " + str);
            }
        }

        public static DateTime ParseDateTime(string str)
        {
            return DateTime.Parse(str);
        }

        public static TimeSpan ParseTime(string str)
        {
            return TimeSpan.Parse(str);
        }

        public static string ToString(bool arg)
        {
            return arg ? "true" : "false";
        }

        public static string ToString(double arg)
        {
            if (double.IsPositiveInfinity(arg))
            {
                return "INF";
            }
            else if (double.IsNegativeInfinity(arg))
            {
                return "-INF";
            }
            else
            {
                return arg.ToString();
            }
        }

        public static string ToString(DateTime arg)
        {
            if (arg.Millisecond == 0)
            {
                return arg.ToString("yyyy-MM-ddTHH:mm:ss");
            }
            else
            {
                return arg.ToString("yyyy-MM-ddTHH:mm:ss.fff");
            }
        }

        public static string ToString(TimeSpan arg)
        {
            DateTime dt = new DateTime(arg.Ticks);
            if (dt.Millisecond == 0)
            {
                return dt.ToString("HH:mm:ss");
            }
            else
            {
                return dt.ToString("HH:mm:ss.fff");
            }
        }
    }

} // protean
