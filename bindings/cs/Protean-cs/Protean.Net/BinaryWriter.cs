using System;
using System.Data;
using System.IO;
using System.IO.Compression;

namespace Protean
{
    public sealed class BinaryWriter : IDisposable
    {
        public BinaryWriter(Stream stream, BinaryMode mode)
        {
            m_mode = mode;

            m_stream = stream;
            if ((m_mode & BinaryMode.Compress) != 0)
            {
                m_filter = new DeflateStream(stream, CompressionMode.Compress, true);
            }
            else
            {
                m_filter = stream;
            }

            m_mode |= BinaryMode.DateTimeAsTicks;

            m_disposed = false;
        }

        public BinaryWriter(Stream stream) :
            this(stream, BinaryMode.Default)
        {}

        public void Dispose()
        {
            Dispose(true);
        }

        private void Dispose(bool disposing)
        {
            // If you need thread safety, use a lock around these 
            // operations, as well as in your methods that use the resource.
            if (!m_disposed)
            {
                if (disposing)
                {
                    if (m_filter != null && (m_mode & BinaryMode.Compress) != 0)
                    {
                        m_filter.Dispose();
                    }
                }

                // Indicate that the instance has been disposed.
                m_filter = null;
                m_disposed = true;
            }
        }

        public static byte[] ToBytes(Variant value, BinaryMode mode)
        {
            using (MemoryStream ms = new MemoryStream())
            {
                using (BinaryWriter writer = new BinaryWriter(ms, mode))
                {
                    writer.Write(value);
                }
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
        }

        public static int GetBytes(Variant value, BinaryMode mode, byte[] buffer, int bufferIndex)
        {
            using (MemoryStream ms = new MemoryStream(buffer, bufferIndex, buffer.Length - bufferIndex))
            {
                using (BinaryWriter writer = new BinaryWriter(ms, mode))
                {
                    writer.Write(value);
                }

                return (int)ms.Position;
            }
        }

        public static int GetByteCount(Variant variant)
        {
            return 12 + GetByteCountVariant(variant);
        }

        private void WriteHeader()
        {
            // Create and write the Protean header
            m_stream.Write(BitConverter.GetBytes(BinaryConstants.PROTEAN_MAGIC), 0, 4);
            m_stream.Write(BitConverter.GetBytes(BinaryConstants.VERSION_MAJOR << 16 | BinaryConstants.VERSION_MINOR), 0, 4);
            m_stream.Write(BitConverter.GetBytes((int)m_mode), 0, 4);
        }

        // This doesn't seem to support Unicode properly
        private void Write(string arg)
        {
            int length = arg.Length;
            byte[] bytes = new byte[length];

            for (int i = 0; i < length; ++i)
            {
                bytes[i] = (byte)arg[i];
            }

            Write(length);
            Write(bytes, true);
        }

        private static int GetByteCountString(string arg)
        {
            return sizeof(Int32) + GetByteCountBytes(arg.Length, true);
        }

        private void Write(TimeSpan arg)
        {
            Write((Int64)arg.TotalMilliseconds);
        }

        private static readonly long MaxDateTimeMillis = (Variant.MaxDateTime.Ticks - Variant.MinDateTime.Ticks) / 10000;

        private void Write(DateTime arg)
        {
            if (arg == DateTime.MaxValue)
            {
                Write(MaxDateTimeMillis);
            }
            else
            {
                Write((arg.Ticks - Variant.MinDateTime.Ticks) / 10000);
            }
        }

        private void Write(byte arg)
        {
            m_filter.WriteByte(arg);
        }

        private void Write(Int32 arg)
        {
            Write(BitConverter.GetBytes(arg));
        }

        private void Write(UInt32 arg)
        {
            Write(BitConverter.GetBytes(arg));
        }

        private void Write(Int64 arg)
        {
            Write(BitConverter.GetBytes(arg));
        }

        private void Write(UInt64 arg)
        {
            Write(BitConverter.GetBytes(arg));
        }

        private void Write(float arg)
        {
            Write(BitConverter.GetBytes(arg));
        }

        private void Write(double arg)
        {
            Write(BitConverter.DoubleToInt64Bits(arg));
        }

        private void Write(byte[] arg)
        {
            m_filter.Write(arg, 0, arg.Length);
        }

        private void Write(bool arg)
        {
            Write((arg ? 1 : 0));
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

        private static int GetByteCountBytes(int byteCount, bool writePadding)
        {
            var residual = writePadding ? ((4 - (byteCount % 4)) % 4) : 0;
            return byteCount + residual;
        }

        private void Write(DataTable arg)
        {
#if !DISABLE_DATATABLE
            int numCols = arg.Columns.Count;
            int numRows = arg.Rows.Count;

            VariantBase.EnumType[] colTypes = new VariantBase.EnumType[numCols];
            string[] colNames = new string[numCols];

            for (int i = 0; i < numCols; ++i)
            {
                DataColumn col = arg.Columns[i];
                colTypes[i] = VariantPrimitiveBase.TypeToEnum(col.DataType);
                colNames[i] = col.ColumnName;
            }

            // Write number of columns
            Write(numCols);

            // Write number of rows
            Write(numRows);

            // Write column types
            foreach (VariantBase.EnumType colType in colTypes)
            {
                Write((int)colType);
            }

            // Write column names
            foreach (string colName in colNames)
            {
                Write(colName);
            }

            // Write columns
            for (int i = 0; i < numCols; ++i)
            {
                WriteDelegate colWriter = GetWriterDelegate(colTypes[i]);

                foreach (DataRow item in arg.Rows)
                {
                    if (item.IsNull(i))
                    {
                        throw new VariantException("Cannot serialise DataTables containing null elements.");
                    }
                    colWriter(item[i]);
                }
            }
#else
            throw new NotSupportedException("Datatables are not supported on this platform.");
#endif
        }

        private static int GetByteCountDataTable(DataTable arg)
        {
#if !DISABLE_DATATABLE
            int numCols = arg.Columns.Count;

            VariantBase.EnumType[] colTypes = new VariantBase.EnumType[numCols];
            string[] colNames = new string[numCols];

            for (int i = 0; i < numCols; ++i)
            {
                DataColumn col = arg.Columns[i];
                colTypes[i] = VariantPrimitiveBase.TypeToEnum(col.DataType);
                colNames[i] = col.ColumnName;
            }

            // Write number of columns
            var writeCount = sizeof(int);

            // Write number of rows
            writeCount += sizeof(int);

            // Write column types
            writeCount += sizeof(int) * colTypes.Length;

            // Write column names
            foreach (string colName in colNames)
            {
                writeCount += GetByteCountString(colName);
            }

            // Write columns
            for (int i = 0; i < numCols; ++i)
            {
                var colWriteCounter = GetWriteCounterDelegate(colTypes[i]);

                foreach (DataRow item in arg.Rows)
                {
                    if (item.IsNull(i))
                    {
                        throw new VariantException("Cannot serialise DataTables containing null elements.");
                    }
                    colWriteCounter(item[i]);
                }
            }

            return writeCount;
#else
            throw new NotSupportedException("Datatables are not supported on this platform.");
#endif
        }

        private void Write(TypedArray arg)
        {
            var writer = GetWriterDelegate(arg.ElementType);

            Write(arg.Count);
            Write((int)arg.ElementType);

            for (int i = 0; i < arg.Count; ++i)
            {
                writer(arg[i]);
            }
        }

        private WriteDelegate GetWriterDelegate(VariantBase.EnumType type)
        {
            WriteDelegate writer;
            switch (type)
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
                    writer = o => Write((int) o);
                    break;
                case VariantBase.EnumType.UInt32:
                    writer = o => Write((uint) o);
                    break;
                case VariantBase.EnumType.Int64:
                    writer = o => Write((long) o);
                    break;
                case VariantBase.EnumType.UInt64:
                    writer = o => Write((ulong) o);
                    break;
                case VariantBase.EnumType.Time:
                    writer = o => Write((TimeSpan) o);
                    break;
                case VariantBase.EnumType.DateTime:
                    writer = o => Write((DateTime) o);
                    break;
                default:
                    throw new VariantException("Case exhaustion: " + type);
            }

            return writer;
        }

        private static int GetByteCountArray(TypedArray arg)
        {
            var counter = GetWriteCounterDelegate(arg.ElementType);

            var count = 2 * sizeof(int); // size + type

            for (int i = 0; i < arg.Count; ++i)
            {
                count += counter(arg[i]);
            }

            return count;
        }

        private static CountDelegate GetWriteCounterDelegate(VariantBase.EnumType type)
        {
            CountDelegate counter;
            switch (type)
            {
                case VariantBase.EnumType.Float:
                    counter = o => sizeof(float);
                    break;
                case VariantBase.EnumType.Double:
                    counter = o => sizeof(double);
                    break;
                case VariantBase.EnumType.String:
                    counter = o => GetByteCountString((string) o);
                    break;
                case VariantBase.EnumType.Boolean:
                    counter = o => sizeof(bool);
                    break;
                case VariantBase.EnumType.Int32:
                    counter = o => sizeof(int);
                    break;
                case VariantBase.EnumType.UInt32:
                    counter = o => sizeof(uint);
                    break;
                case VariantBase.EnumType.Int64:
                    counter = o => sizeof(long);
                    break;
                case VariantBase.EnumType.UInt64:
                    counter = o => sizeof(ulong);
                    break;
                case VariantBase.EnumType.Time:
                    counter = o => sizeof(long);
                    break;
                case VariantBase.EnumType.DateTime:
                    counter = o => sizeof(long);
                    break;
                default:
                    throw new VariantException("Case exhaustion: " + type);
            }

            return counter;
        }

        private delegate void WriteDelegate(object arg);

        private delegate int CountDelegate(object arg);

        private void WriteVariant(Variant v)
        {
            VariantBase.EnumType type = v.Type;
            Write((Int32)type);

            switch (type)
            {
                case VariantBase.EnumType.None:
                    break;
                case VariantBase.EnumType.String:
                case VariantBase.EnumType.Any:
                    Write(v.As<string>());
                    break;
                case VariantBase.EnumType.Float:
                    Write(v.As<float>());
                    break;
                case VariantBase.EnumType.Double:
                    Write(v.As<double>());
                    break;
                case VariantBase.EnumType.Int32:
                    Write(v.As<Int32>());
                    break;
                case VariantBase.EnumType.UInt32:
                    Write(v.As<UInt32>());
                    break;
                case VariantBase.EnumType.Int64:
                    Write(v.As<Int64>());
                    break;
                case VariantBase.EnumType.UInt64:
                    Write(v.As<UInt64>());
                    break;
                case VariantBase.EnumType.Boolean:
                    Write(v.As<bool>());
                    break;
                case VariantBase.EnumType.Time:
                    Write(v.As<TimeSpan>());
                    break;
                case VariantBase.EnumType.DateTime:
                    Write(v.As<DateTime>());
                    break;
                case VariantBase.EnumType.List:
                case VariantBase.EnumType.Tuple:
                    Write(v.Count);
                    foreach (VariantItem item in v)
                    {
                        WriteVariant(item.Value);
                    }
                    break;
                case VariantBase.EnumType.Dictionary:
                case VariantBase.EnumType.Bag:
                    Write(v.Count);
                    foreach (VariantItem item in v)
                    {
                        Write(item.Key);
                        WriteVariant(item.Value);
                    }
                    break;
                case VariantBase.EnumType.TimeSeries:
                    Write(v.Count);
                    foreach (VariantItem item in v)
                    {
                        Write(item.Time);
                        WriteVariant(item.Value);
                    }
                    break;
                case VariantBase.EnumType.Object:
                    IVariantObject o = v.AsObject();
                    Write(o.Class);
                    Write(o.Version);
                    WriteVariant(o.Deflate());
                    break;
                case VariantBase.EnumType.Exception:
                    VariantExceptionInfo x = v.AsException();
                    Write(x.Class);
                    Write(x.Message);
                    Write(x.Source);
                    Write(x.Stack);
                    break;
                case VariantBase.EnumType.Buffer:
                    Write(v.AsBuffer().Length);
                    Write(v.AsBuffer(), true);
                    break;
                case VariantBase.EnumType.DataTable:
                    Write(v.AsDataTable());
                    break;
                case VariantBase.EnumType.Array:
                    Write(v.AsArray());
                    break;
                default:
                    throw new VariantException("Case exhaustion: " + type.ToString());
            }
        }

        private static int GetByteCountVariant(Variant v)
        {
            VariantBase.EnumType type = v.Type;
            var writeCount = sizeof(Int32);

            switch (type)
            {
                case VariantBase.EnumType.None:
                    break;
                case VariantBase.EnumType.String:
                case VariantBase.EnumType.Any:
                    writeCount += GetByteCountString(v.As<string>());
                    break;
                case VariantBase.EnumType.Float:
                    writeCount += sizeof(float);
                    break;
                case VariantBase.EnumType.Double:
                    writeCount += sizeof(double);
                    break;
                case VariantBase.EnumType.Int32:
                    writeCount += sizeof(Int32);
                    break;
                case VariantBase.EnumType.UInt32:
                    writeCount += sizeof(UInt32);
                    break;
                case VariantBase.EnumType.Int64:
                    writeCount += sizeof(Int64);
                    break;
                case VariantBase.EnumType.UInt64:
                    writeCount += sizeof(UInt64);
                    break;
                case VariantBase.EnumType.Boolean:
                    writeCount += sizeof(Int32);
                    break;
                case VariantBase.EnumType.Time:
                    writeCount += sizeof(Int64);
                    break;
                case VariantBase.EnumType.DateTime:
                    writeCount += sizeof(Int64);
                    break;
                case VariantBase.EnumType.List:
                case VariantBase.EnumType.Tuple:
                    writeCount += sizeof(Int32);
                    foreach (VariantItem item in v)
                    {
                        writeCount += GetByteCountVariant(item.Value);
                    }
                    break;
                case VariantBase.EnumType.Dictionary:
                case VariantBase.EnumType.Bag:
                    writeCount += sizeof(Int32);
                    foreach (VariantItem item in v)
                    {
                        writeCount += GetByteCountString(item.Key);
                        writeCount += GetByteCountVariant(item.Value);
                    }
                    break;
                case VariantBase.EnumType.TimeSeries:
                    writeCount += sizeof(Int32);
                    foreach (VariantItem item in v)
                    {
                        writeCount += sizeof(Int64);
                        writeCount += GetByteCountVariant(item.Value);
                    }
                    break;
                case VariantBase.EnumType.Object:
                    IVariantObject o = v.AsObject();
                    writeCount += GetByteCountString(o.Class);
                    writeCount += sizeof(Int32);
                    writeCount += GetByteCountVariant(o.Deflate());
                    break;
                case VariantBase.EnumType.Exception:
                    VariantExceptionInfo x = v.AsException();
                    writeCount += GetByteCountString(x.Class);
                    writeCount += GetByteCountString(x.Message);
                    writeCount += GetByteCountString(x.Source);
                    writeCount += GetByteCountString(x.Stack);
                    break;
                case VariantBase.EnumType.Buffer:
                    writeCount += sizeof(Int32);
                    writeCount += GetByteCountBytes(v.AsBuffer().Length, true);
                    break;
                case VariantBase.EnumType.DataTable:
                    writeCount += GetByteCountDataTable(v.AsDataTable());
                    break;
                case VariantBase.EnumType.Array:
                    writeCount += GetByteCountArray(v.AsArray());
                    break;
                default:
                    throw new VariantException("Case exhaustion: " + type.ToString());
            }

            return writeCount;
        }

        private readonly Stream m_stream;
        private Stream m_filter;
        private readonly BinaryMode m_mode;
        private bool m_disposed;
    }
}

// Protean
