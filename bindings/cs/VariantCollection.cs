//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;

namespace Protean {

    internal class KeyValuePairComparer<TKey, TValue> :
        IComparer<KeyValuePair<TKey, TValue>>
        where TKey : IComparable<TKey>
        where TValue : IComparable<TValue>
    {
        public int Compare(KeyValuePair<TKey, TValue> x, KeyValuePair<TKey, TValue> y)
        {
            if (!x.Key.Equals(y.Key))
            {
                return x.Key.CompareTo(y.Key);
            }
            return x.Value.CompareTo(y.Value);
        }
    }

    internal static class SequenceComparer
    {
        public static int Compare<T>(this IEnumerable<T> a, IEnumerable<T> b) where T : IComparable<T>
        {
            return Compare(a, b, Comparer<T>.Default);
        }

        public static int Compare<T>(this IEnumerable<T> a, IEnumerable<T> b, IComparer<T> comparer)
        {
            if (a == null || b == null || comparer == null)
            {
                throw new InvalidOperationException();
            }

            var valueA = a.GetEnumerator();
            var valueB = b.GetEnumerator();

            bool hasValueA = true;
            bool hasValueB = true;
            while (hasValueA && hasValueB)
            {
                hasValueA = valueA.MoveNext();
                hasValueB = valueB.MoveNext();

                if (hasValueA && hasValueB)
                {
                    int valueCompare = comparer.Compare(valueA.Current, valueB.Current);
                    if (valueCompare != 0)
                    {
                        return valueCompare;
                    }
                }
            }

            if (hasValueA && !hasValueB)
            {
                return 1;
            }
            else if (!hasValueA && hasValueB)
            {
                return -1;
            }
            else
            {
                return 0;
            }
        }
    }

    internal interface IVariantCollection :
        IVariantData
    {
        int Count { get; }

        void Clear();

        IEnumerator<VariantItem> GetEnumerator();
    }

    internal class VariantTimeSeries : IVariantCollection
    {
        public VariantTimeSeries()
        {
            Value = new List<KeyValuePair<DateTime, Variant>>();
        }

        public VariantTimeSeries(int capacity)
        {
            Value = new List<KeyValuePair<DateTime, Variant>>(capacity);
        }

        public VariantTimeSeries(VariantTimeSeries rhs)
        {
            Value = new List<KeyValuePair<DateTime, Variant>>(rhs.Value);
        }

        public int Count
        {
            get { return Value.Count; }
        }

        public void Clear()
        {
            Value.Clear();
        }

        public IEnumerator<VariantItem> GetEnumerator()
        {
            return new VariantEnumerator(((IEnumerable<KeyValuePair<DateTime, Variant>>)Value).GetEnumerator());
        }

        public VariantBase.EnumType _Type
        {
            get { return VariantBase.EnumType.TimeSeries; }
        }

        public int CompareTo(IVariantData rhs)
        {
            return SequenceComparer.Compare(Value, ((VariantTimeSeries)rhs).Value, new KeyValuePairComparer<DateTime, Variant>());
        }

        public List<KeyValuePair<DateTime, Variant>> Value { get; set; }
    }

    //
    // Mapping interface
    //
    internal interface IVariantMapping : IVariantCollection
    {
        bool ContainsKey(string key);
        void Add(string key, Variant value);
        Variant this[string key]
        {
            get; set;
        }
        void Remove(string key);
        Variant Range(string key);
    }

    internal class VariantDictionary : IVariantMapping
    {
        public VariantDictionary()
        {
            Value = new SortedDictionary<String, Variant>();
        }

        public VariantDictionary(VariantDictionary rhs)
        {
            Value = new SortedDictionary<String, Variant>(rhs.Value);
        }

        public int Count
        {
            get { return Value.Count; }
        }

        public void Clear()
        {
            Value.Clear();
        }

        public bool ContainsKey(string key)
        {
            return Value.ContainsKey(key);
        }
        public void Add(string key, Variant value)
        {
            Value.Add(key, value);
        }
        public Variant this[string key]
        {
            get { return Value[key]; }
            set { Value[key] = value; }
        }
        public void Remove(string key)
        {
            Value.Remove(key);
        }
        public Variant Range(string key)
        {
            Variant result = new Variant(Variant.EnumType.List);
            if (ContainsKey(key))
            {
                result.Add(Value[key]);
            }

            return result;
        }

        public IEnumerator<VariantItem> GetEnumerator()
        {
            return new VariantEnumerator(((IEnumerable<KeyValuePair<string, Variant>>)Value).GetEnumerator());
        }

        public VariantBase.EnumType _Type
        {
            get { return VariantBase.EnumType.Dictionary; }
        }

        public int CompareTo(IVariantData rhs)
        {
            return SequenceComparer.Compare(Value, ((VariantDictionary)rhs).Value, new KeyValuePairComparer<string, Variant>());
        }

        // Use a sorted dictionary, so we can easily compare
        public SortedDictionary<string, Variant> Value { get; set; }
    }

    internal class VariantBag : IVariantMapping
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

        public void Clear()
        {
            Value.Clear();
        }

        public bool ContainsKey(string key)
        {
            return System.Linq.Enumerable.Any(Value, kv => kv.Key==key);
        }
        public void Add(string key, Variant value)
        {
            Value.Add(new KeyValuePair<string, Variant>(key, value));
        }

        public Variant this[string key]
        {
            get {
                int index = Value.FindIndex(delegate(KeyValuePair<string, Variant> kv) { return kv.Key==key; });
                if (index==-1)
                {
                    throw new VariantException(string.Format("Key {0} does not Exist in Bag", key));
                }
                return Value[index].Value;
            }

            set {
                int index = Value.FindIndex(delegate(KeyValuePair<string, Variant> kv) { return kv.Key==key; });
                if (index==-1)
                {
                    throw new VariantException(string.Format("Key {0} does not Exist in Bag", key));
                }
                Value[index] = new KeyValuePair<string, Variant>(key, value);
            }
        }

        public void Remove(string key)
        {
            Value.RemoveAll(obj => obj.Key == key);
        }

        public Variant Range(string key)
        {
            Variant result = new Variant(Variant.EnumType.List);
            foreach (KeyValuePair<string, Variant> kv in Value)
            {
                if (kv.Key == key)
                {
                    result.Add(kv.Value);
                }
            }

            return result;
        }

        public IEnumerator<VariantItem> GetEnumerator()
        {
            return new VariantEnumerator(((IEnumerable<KeyValuePair<string, Variant>>)Value).GetEnumerator());
        }

        public int CompareTo(IVariantData rhs)
        {
            return SequenceComparer.Compare(Value, ((VariantBag)rhs).Value, new KeyValuePairComparer<string, Variant>());
        }

        public VariantBase.EnumType _Type
        {
            get { return VariantBase.EnumType.Bag; }
        }

        public List<KeyValuePair<String, Variant>> Value { get; set; }
    }

    //
    // Sequence interface
    //
    internal interface IVariantSequence : IVariantCollection
    {
        Variant this[int index]
        {
            get; set;
        }
    }

    internal class VariantList : IVariantSequence
    {
        public VariantList()
        {
            Value = new List<Variant>();
        }

        public VariantList(int capacity)
        {
            Value = new List<Variant>(capacity);
        }

        public VariantList(VariantList rhs)
        {
            Value = new List<Variant>(rhs.Value);
        }

        public int Count
        {
            get { return Value.Count; }
        }

        public void Clear()
        {
            Value.Clear();
        }

        public void Add(Variant value)
        {
            Value.Add(value);
        }

        public Variant this[int index]
        {
            get {
                if (index < 0 || index >= Value.Count)
                {
                    throw new VariantException(string.Format("Index {0} is out of range for List of size {1}", index, Value.Count));
                }
                return Value[index];
            }

            set {
                if (index < 0 || index >= Value.Count)
                {
                    throw new VariantException(string.Format("Index {0} is out of range for List of size {1}", index, Value.Count));
                }
                Value[index] = value;
            }
        }

        public int CompareTo(IVariantData rhs)
        {
            return SequenceComparer.Compare(Value, ((VariantList)rhs).Value);
        }

        public IEnumerator<VariantItem> GetEnumerator()
        {
            return new VariantEnumerator(((IEnumerable<Variant>)Value).GetEnumerator());
        }

        public VariantBase.EnumType _Type
        {
            get { return VariantBase.EnumType.List; }
        }

        public List<Variant> Value { get; set; }
    }

    internal class VariantTuple : IVariantSequence
    {
        public VariantTuple()
        {
            Value = new Variant[0];
        }

        public VariantTuple(Variant[] value)
        {
            Value = value;
        }

        public VariantTuple(int capacity)
        {
            Value = new Variant[capacity];
            Clear();
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

        public void Clear()
        {
            for (int i=0; i<Value.Length; ++i)
            {
                Value[i] = new Variant();
            }
        }

        public Variant this[int index]
        {
            get
            {
                if (index < 0 || index >= Value.Length)
                {
                    throw new VariantException(string.Format("Index {0} is out of range for List of size {1}", index, Value.Length));
                }
                return Value[index];
            }

            set
            {
                if (index < 0 || index >= Value.Length)
                {
                    throw new VariantException(string.Format("Index {0} is out of range for List of size {1}", index, Value.Length));
                }
                Value[index] = value;
            }
        }

        public int CompareTo(IVariantData rhs)
        {
            return SequenceComparer.Compare(Value, ((VariantTuple)rhs).Value);
        }

        public IEnumerator<VariantItem> GetEnumerator()
        {
            return new VariantEnumerator(((IEnumerable<Variant>)Value).GetEnumerator());
        }

        public VariantBase.EnumType _Type
        {
            get { return VariantBase.EnumType.Tuple; }
        }

        public Variant[] Value { get; set; }
    }

} // Protean
