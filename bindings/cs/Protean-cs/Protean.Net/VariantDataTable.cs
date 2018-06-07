using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;

namespace Protean {

    // Primitives
    internal class VariantDataTable : IVariantData
    {
        public VariantDataTable(DataTable value)
        {
#if !DISABLE_DATATABLE
            Value = value;
#else
            throw new NotSupportedException("Datatables are not supported on this platform.");
#endif
        }

        public VariantBase.EnumType _Type  {
            get { return VariantBase.EnumType.DataTable;  }
        }

        public DataTable Value { get; private set; }

        delegate int CompareDelegate(object lhs, object rhs);

        public int CompareTo(IVariantData value)
        {
#if !DISABLE_DATATABLE
            DataTable rhsValue = ((VariantDataTable)value).Value;

            if (Value.Columns.Count != rhsValue.Columns.Count)
            {
                return Value.Columns.Count.CompareTo(rhsValue.Columns.Count);
            }
            else if (Value.Rows.Count != rhsValue.Rows.Count)
            {
                return Value.Rows.Count.CompareTo(rhsValue.Rows.Count);
            }
            else
            {
                CompareDelegate[] comparers = new CompareDelegate[Value.Columns.Count];

                for (int i = 0; i < Value.Columns.Count; ++i)
                {
                    if (Value.Columns[i].ColumnName != rhsValue.Columns[i].ColumnName)
                    {
                        return Value.Columns[i].ColumnName.CompareTo(rhsValue.Columns[i].ColumnName);
                    }
                    else
                    {
                        Variant.EnumType lhsType = VariantPrimitiveBase.TypeToEnum(Value.Columns[i].DataType);
                        Variant.EnumType rhsType = VariantPrimitiveBase.TypeToEnum(rhsValue.Columns[i].DataType);

                        if (lhsType != rhsType)
                        {
                            return lhsType.CompareTo(rhsType);
                        }
                        else
                        {
                            switch(lhsType)
                            {
                                case VariantBase.EnumType.Float:
                                    comparers[i] = delegate(object lhs, object rhs) { return ((float)lhs).CompareTo((float)rhs); };
                                    break;
                                case VariantBase.EnumType.Double:
                                    comparers[i] = delegate(object lhs, object rhs) { return ((double)lhs).CompareTo((double)rhs); };
                                    break;
                                case VariantBase.EnumType.Boolean:
                                    comparers[i] = delegate(object lhs, object rhs) { return ((bool)lhs).CompareTo((bool)rhs); };
                                    break;
                                case VariantBase.EnumType.String:
                                    comparers[i] = delegate(object lhs, object rhs) { return ((string)lhs).CompareTo((string)rhs); };
                                    break;
                                case VariantBase.EnumType.Int32:
                                    comparers[i] = delegate(object lhs, object rhs) { return ((Int32)lhs).CompareTo((Int32)rhs); };
                                    break;
                                case VariantBase.EnumType.UInt32:
                                    comparers[i] = delegate(object lhs, object rhs) { return ((UInt32)lhs).CompareTo((UInt32)rhs); };
                                    break;
                                case VariantBase.EnumType.Int64:
                                    comparers[i] = delegate(object lhs, object rhs) { return ((Int64)lhs).CompareTo((Int64)rhs); };
                                    break;
                                case VariantBase.EnumType.UInt64:
                                    comparers[i] = delegate(object lhs, object rhs) { return ((UInt64)lhs).CompareTo((UInt64)rhs); };
                                    break;
                                case VariantBase.EnumType.Time:
                                    comparers[i] = delegate(object lhs, object rhs) { return ((TimeSpan)lhs).CompareTo((TimeSpan)rhs); };
                                    break;
                                case VariantBase.EnumType.DateTime:
                                    comparers[i] = delegate(object lhs, object rhs) { return ((DateTime)lhs).CompareTo((DateTime)rhs); };
                                    break;
                                default:
                                    throw new VariantException("Case exhaustion: " + lhsType);
                            }
                        }
                    }
                }

                for (int i = 0; i < Value.Rows.Count; ++i)
                {
                    DataRow lhsRow = Value.Rows[i];
                    DataRow rhsRow = rhsValue.Rows[i];
                    for (int j = 0; j < Value.Columns.Count; ++j)
                    {
                        int cmp = comparers[j](lhsRow[j], rhsRow[j]);
                        if (cmp != 0)
                        {
                            return cmp;
                        }
                    }
                }

                return 0;

            }
#else
            throw new NotSupportedException("Datatables are not supported on this platform.");
#endif
        }

        public override int GetHashCode()
        {
            unchecked
            {
                var hashCode = 0x2D2816FE;
                foreach (DataColumn column in Value.Columns)
                {
                    hashCode = (hashCode * 397) ^ EqualityComparer<string>.Default.GetHashCode(column.ColumnName);
                }
                foreach (DataRow row in Value.Rows)
                {
                    hashCode = row.ItemArray.Aggregate(hashCode, (cur, item) => (cur * 397) ^ item.GetHashCode());
                }

                return hashCode;
            }
        }
    }

} // Protean
