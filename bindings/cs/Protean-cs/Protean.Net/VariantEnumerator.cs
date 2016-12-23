//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;

namespace Protean {

    public class DummyEnumerator :
        IEnumerator<VariantItem>
    {
        public bool MoveNext()
        {
            return false;
        }

        public void Reset()
        { }

        public VariantItem Current
        {
            get { throw new VariantException("Attempt to dereference dummy iterator"); }
        }

        Object System.Collections.IEnumerator.Current
        {
            get { return Current; }
        }

        void IDisposable.Dispose() { }
    }

    public class VariantEnumerator :
        IEnumerator<VariantItem>
    {
        private VariantEnumerator(Variant.EnumType type, System.Collections.IEnumerator enumerator)
        {
            Type = type;
            m_enumerator = enumerator;
        }
        public VariantEnumerator(IEnumerator<Variant> enumerator) :
            this(Variant.EnumType.Sequence, enumerator)
        {  }

        public VariantEnumerator(IEnumerator<KeyValuePair<String, Variant>> enumerator) :
            this(Variant.EnumType.Mapping, enumerator)
        { }

        public VariantEnumerator(IEnumerator<KeyValuePair<DateTime, Variant>> enumerator) :
            this(Variant.EnumType.TimeSeries, enumerator)
        { }

        public bool MoveNext()
        {
            return m_enumerator.MoveNext();
        }

        public void Reset()
        {
            m_enumerator.Reset();
        }

        public Variant.EnumType Type { get; private set; }

        public VariantItem Current
        {
            get
            {
                switch (Type)
                {
                    case Variant.EnumType.Sequence:
                    {
                        IEnumerator<Variant> enumerator = (IEnumerator<Variant>)m_enumerator;
                        return new VariantItem(enumerator.Current);
                    }
                    case Variant.EnumType.Mapping:
                    {
                        IEnumerator<KeyValuePair<String, Variant>> enumerator = m_enumerator as IEnumerator<KeyValuePair<String, Variant>>;
                        return new VariantItem(enumerator.Current.Key, enumerator.Current.Value);
                    }
                    case Variant.EnumType.TimeSeries:
                    {
                        IEnumerator<KeyValuePair<DateTime, Variant>> enumerator = m_enumerator as IEnumerator<KeyValuePair<DateTime, Variant>>;
                        return new VariantItem(enumerator.Current.Key, enumerator.Current.Value);
                    }
                    default:
                        throw new VariantException("Case exhaustion: " + Type.ToString());
                }
            }
        }

        Object System.Collections.IEnumerator.Current
        {
            get { return Current; }
        }

        void IDisposable.Dispose() { }
        
        private System.Collections.IEnumerator m_enumerator;
    }

} // Protean
