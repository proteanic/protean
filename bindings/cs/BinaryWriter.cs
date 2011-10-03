//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Data;

namespace Protean {

    public class BinaryWriter
    {
        public BinaryWriter(System.IO.Stream stream, BinaryMode mode)
        {
            Mode = mode;

            Stream = stream;
            if ((Mode & BinaryMode.Compress) != 0)
            {
                Filter = new System.IO.Compression.DeflateStream(stream, System.IO.Compression.CompressionMode.Compress, true);
            }
            else
            {
                Filter = stream;
            }

            Mode |= BinaryMode.DateTimeAsTicks;
        }

        public BinaryWriter(System.IO.Stream stream) :
            this(stream, BinaryMode.Default)
        { }

        public static byte[] ToBytes(Variant value, BinaryMode mode)
        {
            using (System.IO.MemoryStream ms = new System.IO.MemoryStream())
            {
                BinaryWriter writer = new BinaryWriter(ms, mode);
                writer.Write(value);
                return ms.ToArray();
            }
        }

        public static byte[] ToBytes(Variant value)
        {
            return ToBytes(value, BinaryMode.Default);
        }

        public void Write(Variant v)
        {
            WriteHeader();
            WriteVariant(v);
            Finish();
        }

        private void WriteHeader()
        {
            // Create and write the Protean header
            Stream.Write(System.BitConverter.GetBytes(BinaryConstants.PROTEAN_MAGIC), 0, 4);
            Stream.Write(System.BitConverter.GetBytes(BinaryConstants.VERSION_MAJOR << 16 | BinaryConstants.VERSION_MINOR), 0, 4);
            Stream.Write(System.BitConverter.GetBytes((int)Mode), 0, 4);
        }

        private void Finish()
        {
            if ((Mode & BinaryMode.Compress) != 0)
            {
                Filter.Close();
            }
            Filter = null;
        }

        private void Write(string arg)
        {
            byte[] bytes = System.Text.Encoding.ASCII.GetBytes(arg);
            Write(bytes.Length);
            Write(bytes, true);
        }

        private void Write(TimeSpan arg)
        {
            Write((Int64)arg.TotalMilliseconds);
        }

        static readonly long MaxDateTimeMillis = (Variant.MaxDateTime.Ticks - Variant.MinDateTime.Ticks)/10000;

        private void Write(DateTime arg)
        {
            if (arg == DateTime.MaxValue)
            {
                Write(MaxDateTimeMillis);
            }
            else
            {
                Write((arg.Ticks - Variant.MinDateTime.Ticks)/10000);
            }
        }

        private void Write(byte arg)
        {
            Filter.WriteByte(arg);
        }

        private void Write(Int32 arg)
        {
            Write(System.BitConverter.GetBytes(arg));
        }

        private void Write(UInt32 arg)
        {
            Write(System.BitConverter.GetBytes(arg));
        }

        private void Write(Int64 arg)
        {
            Write(System.BitConverter.GetBytes(arg));
        }

        private void Write(UInt64 arg)
        {
            Write(System.BitConverter.GetBytes(arg));
        }

        private void Write(float arg)
        {
            Write(System.BitConverter.GetBytes(arg));
        }

        private void Write(double arg)
        {
            Write(System.BitConverter.DoubleToInt64Bits(arg));
        }

        private void Write(byte[] arg)
        {
            Filter.Write(arg, 0, arg.Length);
        }

        private void Write(bool arg)
        {
            Write((int)(arg ? 1 : 0));
        }

        private void Write(byte[] bytes, bool writePadding)
        {
            Write(bytes);

            if (writePadding)
            {
                int residual = (4 - (bytes.Length % 4)) % 4;
                for (int i = 0; i < residual; ++i)
                {
                    Write((byte)0);
                }
            }
        }

        private void Write(DataTable arg)
        {
            int numCols = arg.Columns.Count;
            int numRows = arg.Rows.Count;

            WriteDelegate[] colWriters = new WriteDelegate[numCols];
            Variant.EnumType[] colTypes = new Variant.EnumType[numCols];
            string[] colNames = new string[numCols];

            for (int i = 0; i < numCols; ++i)
            {
                DataColumn col = arg.Columns[i];
                colTypes[i] = VariantPrimitiveBase.TypeToEnum(col.DataType);
                colNames[i] = col.ColumnName;

                switch (colTypes[i])
                {
                    case VariantBase.EnumType.Float:
                        colWriters[i] = o => Write((float) o);
                        break;
                    case VariantBase.EnumType.Double:
                        colWriters[i] = o => Write((double) o);
                        break;
                    case VariantBase.EnumType.String:
                        colWriters[i] = o => Write((string) o);
                        break;
                    case VariantBase.EnumType.Boolean:
                        colWriters[i] = o => Write((bool) o);
                        break;
                    case VariantBase.EnumType.Int32:
                        colWriters[i] = o => Write((Int32) o);
                        break;
                    case VariantBase.EnumType.UInt32:
                        colWriters[i] = o => Write((UInt32) o);
                        break;
                    case VariantBase.EnumType.Int64:
                        colWriters[i] = o => Write((Int64) o);
                        break;
                    case VariantBase.EnumType.UInt64:
                        colWriters[i] = o => Write((UInt64) o);
                        break;
                    case VariantBase.EnumType.Time:
                        colWriters[i] = o => Write((TimeSpan) o);
                        break;
                    case VariantBase.EnumType.DateTime:
                        colWriters[i] = o => Write((DateTime) o);
                        break;
                    default:
                        throw new VariantException("Case exhaustion: " + colTypes[i]);
                }
            }

            // Write number of columns
            Write(numCols);

            // Write number of rows
            Write(numRows);

            // Write column types
            foreach (Variant.EnumType colType in colTypes)
            {
                Write((int)colType);
            }

            // Write column names
            foreach (string colName in colNames)
            {
                Write(colName);
            }

            // Write rows
            foreach (DataRow item in arg.Rows)
            {
                for (int i = 0; i < numCols; ++i)
                {
                    if (item.IsNull(i))
                    {
                        throw new VariantException("Cannot serialise DataTables containing null elements.");
                    }
                    colWriters[i](item[i]);
                }
            }
        }

        private void Write(TypedArray arg)
        {
            WriteDelegate writer = null;
            switch (arg.ElementType)
            {
                case VariantBase.EnumType.Float:
                    writer = o => Write((float) o);
                    break;
                case VariantBase.EnumType.Double:
                    writer = o => Write((double) o);
                    break;
                case VariantBase.EnumType.String:
                    writer = o => Write((string) o);
                    break;
                case VariantBase.EnumType.Boolean:
                    writer = o => Write((bool) o);
                    break;
                case VariantBase.EnumType.Int32:
                    writer = o => Write((Int32) o);
                    break;
                case VariantBase.EnumType.UInt32:
                    writer = o => Write((UInt32) o);
                    break;
                case VariantBase.EnumType.Int64:
                    writer = o => Write((Int64) o);
                    break;
                case VariantBase.EnumType.UInt64:
                    writer = o => Write((UInt64) o);
                    break;
                case VariantBase.EnumType.Time:
                    writer = o => Write((TimeSpan) o);
                    break;
                case VariantBase.EnumType.DateTime:
                    writer = o => Write((DateTime) o);
                    break;
                default:
                    throw new VariantException("Case exhaustion: " + arg.ElementType);
            }

            Write(arg.Count);
            Write((int)arg.ElementType);

            for (int i = 0; i < arg.Count; ++i)
            {
                writer(arg[i]);
            }
        }

        delegate void WriteDelegate(object arg);

        private void WriteVariant(Variant v)
        {
            Variant.EnumType type = v.Type;
            Write((Int32)type);

            switch (type)
            {
            case Variant.EnumType.None:
                break;
            case Variant.EnumType.String:
            case Variant.EnumType.Any:
                Write(v.As<string>());
                break;
            case Variant.EnumType.Float:
                Write(v.As<float>());
                break;
            case Variant.EnumType.Double:
                Write(v.As<double>());
                break;
            case Variant.EnumType.Int32:
                Write(v.As<Int32>());
                break;
            case Variant.EnumType.UInt32:
                Write(v.As<UInt32>());
                break;
            case Variant.EnumType.Int64:
                Write(v.As<Int64>());
                break;
            case Variant.EnumType.UInt64:
                Write(v.As<UInt64>());
                break;
            case Variant.EnumType.Boolean:
                Write(v.As<bool>());
                break;
            case Variant.EnumType.Time:
                Write(v.As<TimeSpan>());
                break;
            case Variant.EnumType.DateTime:
                Write(v.As<DateTime>());
                break;
            case Variant.EnumType.List:
            case Variant.EnumType.Tuple:
                Write(v.Count);
                foreach (VariantItem item in v)
                {
                    WriteVariant(item.Value);
                }
                break;
            case Variant.EnumType.Dictionary:
            case Variant.EnumType.Bag:
                Write(v.Count);
                foreach (VariantItem item in v)
                {
                    Write(item.Key);
                    WriteVariant(item.Value);
                }
                break;
            case Variant.EnumType.TimeSeries:
                Write(v.Count);
                foreach (VariantItem item in v)
                {
                    Write(item.Time);
                    WriteVariant(item.Value);
                }
                break;
            case Variant.EnumType.Object:
                IVariantObject o = v.AsObject();
                Write(o.Class);
                Write(o.Version);
                WriteVariant(o.Deflate());
                break;
            case Variant.EnumType.Exception:
                VariantExceptionInfo x = v.AsException();
                Write(x.Class);
                Write(x.Message);
                Write(x.Source);
                Write(x.Stack);
                break;
            case Variant.EnumType.Buffer:
                Write(v.AsBuffer().Length);
                Write(v.AsBuffer(), true);
                break;
            case Variant.EnumType.DataTable:
                Write(v.AsDataTable());
                break;
            case Variant.EnumType.Array:
                Write(v.AsArray());
                break;
            default:
                throw new VariantException("Case exhaustion: " + type.ToString());
            }
        }

        private readonly System.IO.Stream Stream;
        private System.IO.Stream Filter;
        private readonly BinaryMode Mode;
    }
} // Protean
