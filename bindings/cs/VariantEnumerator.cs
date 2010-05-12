//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Text;

namespace protean {
    
    public class VariantEnumerator :
        IEnumerator<VariantItem>
    {
        public VariantEnumerator(Variant.EnumType type, System.Collections.IEnumerable enumerable)
        {
            Type = type;
            m_enumerator = enumerable.GetEnumerator();
        }

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
		            case Variant.EnumType.List:
			        {
			            IEnumerator<Variant> enumerator = m_enumerator as IEnumerator<Variant>;
			            Variant value = enumerator.Current;
			            return new VariantItem(value);
			        }
		            case Variant.EnumType.Dictionary:
		            case Variant.EnumType.Bag:
			        {
			            IEnumerator<KeyValuePair<String, Variant>> enumerator = m_enumerator as IEnumerator<KeyValuePair<String, Variant>>;
			            String key = enumerator.Current.Key;
			            Variant value = enumerator.Current.Value;
			            return new VariantItem(key, value);
			        }
		            case Variant.EnumType.TimeSeries:
			        {
			            IEnumerator<KeyValuePair<DateTime, Variant>> enumerator = m_enumerator as IEnumerator<KeyValuePair<DateTime, Variant>>;
			            DateTime time = enumerator.Current.Key;
			            Variant value = enumerator.Current.Value;
			            return new VariantItem(time, value);
			        }
		            default:
			            throw new VariantException("Case exhaustion: " + Type.ToString());
		        }
            }
        }

        Object System.Collections.IEnumerator.Current
        {
            get
            {
                return Current;
            }
        }

        void IDisposable.Dispose() { }
        
        private Variant m_parent;
        private System.Collections.IEnumerator m_enumerator;
    }

} // protean
