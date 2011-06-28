//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;

namespace Protean
{
    public sealed class TypedArray : IVariantData
    {
        public TypedArray(VariantBase.EnumType elementType, int size)
        {
            ElementType = elementType;
            Value = new Object[size];
        }

        public TypedArray(TypedArray rhs)
        {
            ElementType = rhs.ElementType;
            Value = new Object[rhs.Value.Length];
        }

        public int Count
        {
            get { return Value.Length; }
        }

        public Object this[int index]
        {
            get
            {
                if (index < 0 || index >= Value.Length)
                {
                    throw new VariantException(string.Format("Index {0} is out of range for Array of size {1}", index, Value.Length));
                }
                return Value[index];
            }

            set
            {
                if (index < 0 || index >= Value.Length)
                {
                    throw new VariantException(string.Format("Index {0} is out of range for Array of size {1}", index, Value.Length));
                }
                Value[index] = value;
            }
        }

        delegate int CompareDelegate(object lhs, object rhs);

        public int CompareTo(IVariantData value)
        {
            TypedArray rhsValue = ((TypedArray)value);

            if (Value.Length != rhsValue.Value.Length)
            {
                return Value.Length.CompareTo(rhsValue.Value.Length);
            }
            else if (ElementType != rhsValue.ElementType)
            {
                return ElementType.CompareTo(rhsValue.ElementType);
            }
            else
            {
                CompareDelegate comparer = null;

                switch (ElementType)
                {
                    case VariantBase.EnumType.Float:
                        comparer = delegate(object lhs, object rhs) { return ((float)lhs).CompareTo((float)rhs); };
                        break;
                    case VariantBase.EnumType.Double:
                        comparer = delegate(object lhs, object rhs) { return ((double)lhs).CompareTo((double)rhs); };
                        break;
                    case VariantBase.EnumType.Boolean:
                        comparer = delegate(object lhs, object rhs) { return ((bool)lhs).CompareTo((bool)rhs); };
                        break;
                    case VariantBase.EnumType.String:
                        comparer = delegate(object lhs, object rhs) { return ((string)lhs).CompareTo((string)rhs); };
                        break;
                    case VariantBase.EnumType.Int32:
                        comparer = delegate(object lhs, object rhs) { return ((Int32)lhs).CompareTo((Int32)rhs); };
                        break;
                    case VariantBase.EnumType.UInt32:
                        comparer = delegate(object lhs, object rhs) { return ((UInt32)lhs).CompareTo((UInt32)rhs); };
                        break;
                    case VariantBase.EnumType.Int64:
                        comparer = delegate(object lhs, object rhs) { return ((Int64)lhs).CompareTo((Int64)rhs); };
                        break;
                    case VariantBase.EnumType.UInt64:
                        comparer = delegate(object lhs, object rhs) { return ((UInt64)lhs).CompareTo((UInt64)rhs); };
                        break;
                    case VariantBase.EnumType.Time:
                        comparer = delegate(object lhs, object rhs) { return ((TimeSpan)lhs).CompareTo((TimeSpan)rhs); };
                        break;
                    case VariantBase.EnumType.DateTime:
                        comparer = delegate(object lhs, object rhs) { return ((DateTime)lhs).CompareTo((DateTime)rhs); };
                        break;
                    default:
                        throw new VariantException("Case exhaustion: " + ElementType);
                }

                for (int i = 0; i < Value.Length; ++i)
                {
                    int cmp = comparer(Value[i], rhsValue.Value[i]);
                    if (cmp != 0)
                    {
                        return cmp;
                    }
                }
            }
            return 0;
        }

        public VariantBase.EnumType _Type
        {
            get { return VariantBase.EnumType.Array; }
        }

        public VariantBase.EnumType ElementType { get; private set; }
        public Object[] Value { get; set; }
    }
}
