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

        void Clear();

        IEnumerator<VariantItem> GetEnumerator();
    }

    public class VariantTimeSeries : IVariantCollection
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
            return new VariantEnumerator(Type, Value);
        }

        public VariantBase.EnumType Type
        {
            get { return VariantBase.EnumType.TimeSeries; }
        }

        public List<KeyValuePair<DateTime, Variant>> Value { get; set; }
    }

    //
    // Mapping interface
    //
    public interface IVariantMapping : IVariantCollection
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

    public class VariantDictionary : IVariantMapping
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
                result.Add(key, Value[key]);
            }

            return result;
        }

        public IEnumerator<VariantItem> GetEnumerator()
        {
            return new VariantEnumerator(Type, Value);
        }

        public VariantBase.EnumType Type
        {
            get { return VariantBase.EnumType.Dictionary; }
        }

        public Dictionary<String, Variant> Value { get; set; }
    }

    public class VariantBag : IVariantMapping
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
            return new VariantEnumerator(Type, Value);
        }

        public VariantBase.EnumType Type
        {
            get { return VariantBase.EnumType.Bag; }
        }

        public List<KeyValuePair<String, Variant>> Value { get; set; }
    }

    //
    // Sequence interface
    //
    public interface IVariantSequence : IVariantCollection
    {
        Variant this[int index]
        {
            get; set;
        }
    }

    public class VariantList : IVariantSequence
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

        public IEnumerator<VariantItem> GetEnumerator()
        {
            return new VariantEnumerator(Type, Value);
        }

        public VariantBase.EnumType Type
        {
            get { return VariantBase.EnumType.List; }
        }

        public List<Variant> Value { get; set; }
    }

    public class VariantTuple : IVariantSequence
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

        public IEnumerator<VariantItem> GetEnumerator()
        {
            return new VariantEnumerator(Type, Value);
        }

        public VariantBase.EnumType Type
        {
            get { return VariantBase.EnumType.Tuple; }
        }

        public Variant[] Value { get; set; }
    }

} // protean
