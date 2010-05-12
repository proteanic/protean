//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Text;

namespace protean {

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

	    [System.Runtime.InteropServices.StructLayout(System.Runtime.InteropServices.LayoutKind.Explicit)]
	    private struct VariantUnion
	    {
		    [System.Runtime.InteropServices.FieldOffset(0)]
		    public bool BooleanValue; 
		    [System.Runtime.InteropServices.FieldOffset(0)]
		    public UInt32 UInt32Value; 
		    [System.Runtime.InteropServices.FieldOffset(0)]
		    public Int32 Int32Value; 
		    [System.Runtime.InteropServices.FieldOffset(0)]
		    public UInt64 UInt64Value; 
		    [System.Runtime.InteropServices.FieldOffset(0)]
		    public Int64 Int64Value; 
		    [System.Runtime.InteropServices.FieldOffset(0)]
		    public float FloatValue; 
		    [System.Runtime.InteropServices.FieldOffset(0)]
		    public double DoubleValue; 
		    [System.Runtime.InteropServices.FieldOffset(0)]
		    public TimeSpan TimeValue; 
		    [System.Runtime.InteropServices.FieldOffset(0)]
		    public DateTime DateTimeValue; 
	    };

        private VariantUnion m_value;
        private Object m_reference;

	    protected void Initialise(EnumType type, UInt32 size)
        {
		    if ((int)(type & EnumType.Number)!=0)
		    {
			    m_value.UInt64Value = 0;
		    }
		    else
		    {
			    switch (type)
			    {
                case EnumType.Date:
			    case EnumType.DateTime:
				    DateTime = new DateTime(1400, 1, 1);
                    break;
                case EnumType.Time:
				    Time = new TimeSpan(0, 0, 0);
                    break;
                case EnumType.TimeSeries:
                    TimeSeries = new List<KeyValuePair<DateTime, Variant>>((int)size);
                    break;
                case EnumType.Any:
                case EnumType.String:
				    String = "";
                    break;
                case EnumType.Dictionary:
                    Dictionary = new Dictionary<String, Variant>();
                    break;
                case EnumType.Bag:
				    Bag = new List<KeyValuePair<String, Variant>>((int)size);
                    break;
                case EnumType.List:
                    List = new List<Variant>((int)size);
                    break;
                case EnumType.Tuple:
                    Tuple = new Variant[size];
                    break;
			    case EnumType.Buffer:
				    Buffer = new byte[size];
                    break;
			    default:
				    throw new VariantException("Cannot default construct variant of type: " + type.ToString());
			    }
		    }
	    }

	    protected void Dispose(EnumType type)
	    {
		    m_reference = null;
	    }

        protected void Assign(EnumType type, VariantBase value)
        {
		    if ((int)(type & EnumType.Number)!=0)
		    {
			    m_value = value.m_value;
		    }
		    else
		    {
			    switch (type)
			    {
                case EnumType.Date:
			    case EnumType.DateTime:
				    DateTime = value.m_value.DateTimeValue;
                    break;
                case EnumType.Time:
				    Time = value.m_value.TimeValue;
                    break;
                case EnumType.TimeSeries:
				    TimeSeries = new List<KeyValuePair<DateTime, Variant>>(value.m_reference as List<KeyValuePair<DateTime, Variant>>);
                    break;
                case EnumType.Any:
                case EnumType.String:
				    String = value.String.Clone() as String;
                    break;
                case EnumType.Dictionary:
				    Dictionary = new Dictionary<String, Variant>(value.m_reference as Dictionary<String, Variant>);
                    break;
                case EnumType.Bag:
				    Bag = new List<KeyValuePair<String, Variant>>(value.m_reference as List<KeyValuePair<String, Variant>>);
                    break;
                case EnumType.List:
				    List = new List<Variant>(value.m_reference as List<Variant>);
                    break;
                case EnumType.Tuple:
                    Tuple = value.Tuple.Clone() as Variant[];
                    break;
			    case EnumType.Buffer:
				    Buffer = value.Buffer.Clone() as byte[];
                    break;
                case EnumType.Object:
                    // TODO
                    Object = value.m_reference as IVariantObject;
                    break;
                case EnumType.Exception:
                    // TODO
                    Exception = value.m_reference as ExceptionInfo;
                    break;
                default:
				    throw new System.ApplicationException("Cannot default construct variant of type: " + type.ToString());
			    }
		    }
        }

        protected String String
        {
            get { return m_reference as String; }
            set { m_reference = value; }
        }
        protected bool Boolean
        {
            get { return m_value.BooleanValue; }
            set { m_value.BooleanValue = value; }
        }
        protected Int32 Int32
        {
            get { return m_value.Int32Value; }
            set { m_value.Int32Value = value; }
        }
        protected UInt32 UInt32
        {
            get { return m_value.UInt32Value; }
            set { m_value.UInt32Value = value; }
        }
        protected Int64 Int64
        {
            get { return m_value.Int64Value; }
            set { m_value.Int64Value = value; }
        }
        protected UInt64 UInt64
        {
            get { return m_value.UInt64Value; }
            set { m_value.UInt64Value = value; }
        }
        protected float Float
        {
            get { return m_value.FloatValue; }
            set { m_value.FloatValue = value; }
        }
        protected double Double
        {
            get { return m_value.DoubleValue; }
            set { m_value.DoubleValue = value; }
        }
        protected DateTime Date
        {
            get { return m_value.DateTimeValue; }
            set { m_value.DateTimeValue = value; }
        }
        protected DateTime DateTime
        {
            get { return m_value.DateTimeValue; }
            set { m_value.DateTimeValue = value; }
        }
        protected TimeSpan Time
        {
            get { return m_value.TimeValue; }
            set { m_value.TimeValue = value; }
        }
        protected List<Variant> List
        {
            get { return m_reference as List<Variant>; }
            set { m_reference = value; }
        }
        protected Dictionary<String, Variant> Dictionary
        {
            get { return m_reference as Dictionary<String, Variant>; }
            set { m_reference = value; }
        }
        protected List<KeyValuePair<String, Variant>> Bag
        {
            get { return m_reference as List<KeyValuePair<String, Variant>>; }
            set { m_reference = value; }
        }
        protected List<KeyValuePair<DateTime, Variant>> TimeSeries
        {
            get { return m_reference as List<KeyValuePair<DateTime, Variant>>; }
            set { m_reference = value; }
        }
        protected Variant[] Tuple
        {
            get { return m_reference as Variant[]; }
            set { m_reference = value; }
        }
        protected byte[] Buffer
        {
            get { return m_reference as byte[]; }
            set { m_reference = value; }
        }
        protected IVariantObject Object
        {
            get { return m_reference as IVariantObject; }
            set { m_reference = value; }
        }
        protected ExceptionInfo Exception
        {
            get { return m_reference as ExceptionInfo; }
            set { m_reference = value; }
        }

        protected System.Collections.ICollection Collection
        {
            get { return m_reference as System.Collections.ICollection; }
        }

        protected System.Collections.IEnumerable Enumerable
        {
            get { return m_reference as System.Collections.IEnumerable; }
        }
    }

} // protean
