//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;

namespace Protean {

    public class VariantItem
    {
        public VariantItem(Variant value)
        {
            Type = Variant.EnumType.Sequence;
            Value = value;
        }

        public VariantItem(String key, Variant value)
        {
            Type = Variant.EnumType.Mapping;
            Key = key;
            Value = value;
        }

        public VariantItem(DateTime time, Variant value)
        {
            Type = Variant.EnumType.TimeSeries;
            Time = time;
            Value = value;
        }

        public Variant.EnumType Type { get; private set; }

        public Variant Value { get; private set; }

        public String Key
        {
            get
            {
                if (Type!=Variant.EnumType.Mapping)
                {
                    throw new VariantException("Attempt to get Key on item that is not a child of an associative container");
                }
                return m_key;
            }
            private set
            {
                m_key = value;
            }
        }
        private String m_key;

        public DateTime Time
        {
            get
            {
                if (Type!=Variant.EnumType.TimeSeries)
                {
                    throw new VariantException("Attempt to get Time on item that is not a child of a timeseries container");
                }
                return m_time;
            }
            private set
            {
                m_time = value;
            }
        }
        private DateTime m_time;

        public override String ToString()
        {
            switch (Type)
            {
                case Variant.EnumType.Mapping:
                    return String.Format(" '{0}':{1} ", Key, Value);
                case Variant.EnumType.TimeSeries:
                    return String.Format(" '{0}':{1} ", Time, Value);
                default:
                    return String.Format(" {0} ", Value);
            }
        }
    }

} // Protean
