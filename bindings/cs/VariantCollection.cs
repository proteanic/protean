//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Text;

namespace protean {

    public interface IVariantCollection : IVariantData
    {
        int Count { get; }
    }

    public class VariantTimeSeries : IVariantCollection
    {
        public VariantTimeSeries()
        {
            Value = new List<KeyValuePair<DateTime, Variant>>();
        }

        public VariantTimeSeries(UInt32 capacity)
        {
            Value = new List<KeyValuePair<DateTime, Variant>>((int)capacity);
        }

        public VariantTimeSeries(VariantTimeSeries rhs)
        {
            Value = new List<KeyValuePair<DateTime, Variant>>(rhs.Value);
        }

        public int Count
        {
            get { return Value.Count; }
        }

        public VariantBase.EnumType Type
        {
            get { return VariantBase.EnumType.TimeSeries; }
        }

        public List<KeyValuePair<DateTime, Variant>> Value { get; set; }
    }

    public class VariantDictionary : IVariantCollection
    {
        public VariantDictionary()
        {
            Value = new Dictionary<String, Variant>();
        }

        public VariantDictionary(VariantDictionary rhs)
        {
            Value = new Dictionary<String, Variant>(rhs.Value);
        }

        public int Count
        {
            get { return Value.Count; }
        }

        public VariantBase.EnumType Type
        {
            get { return VariantBase.EnumType.Dictionary; }
        }

        public Dictionary<String, Variant> Value { get; set; }
    }

    public class VariantList : IVariantCollection
    {
        public VariantList()
        {
            Value = new List<Variant>();
        }

        public VariantList(UInt32 capacity)
        {
            Value = new List<Variant>((int)capacity);
        }

        public VariantList(VariantList rhs)
        {
            Value = new List<Variant>(rhs.Value);
        }

        public int Count
        {
            get { return Value.Count; }
        }

        public VariantBase.EnumType Type
        {
            get { return VariantBase.EnumType.List; }
        }

        public List<Variant> Value { get; set; }
    }

    public class VariantTuple : IVariantCollection
    {
        public VariantTuple()
        {
            Value = new Variant[0];
        }

        public VariantTuple(Variant[] value)
        {
            Value = value;
        }

        public VariantTuple(UInt32 capacity)
        {
            Value = new Variant[capacity];
        }

        public VariantTuple(VariantTuple rhs)
        {
            Value = new Variant[rhs.Count];
            Array.Copy(rhs.Value, Value, rhs.Count);
        }

        public int Count
        {
            get { return Value.Length; }
        }

        public VariantBase.EnumType Type
        {
            get { return VariantBase.EnumType.Tuple; }
        }

        public Variant[] Value { get; set; }
    }

    public class VariantBag : IVariantCollection
    {
        public VariantBag()
        {
            Value = new List<KeyValuePair<String, Variant>>();
        }

        public VariantBag(VariantBag rhs)
        {
            Value = new List<KeyValuePair<String, Variant>>(rhs.Value);
        }

        public int Count
        {
            get { return Value.Count; }
        }

        public VariantBase.EnumType Type
        {
            get { return VariantBase.EnumType.Bag; }
        }

        public List<KeyValuePair<String, Variant>> Value { get; set; }
    }

} // protean
