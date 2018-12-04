using System;
using System.Data;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Text;

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

        public static int GetBytes(Variant value, BinaryMode mode, byte[] buffer, int bufferOffset)
        {
            using (var ms = new MemoryStream(buffer, bufferOffset, buffer.Length - bufferOffset))
            {
                using (var writer = new BinaryWriter(ms, mode))
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

        public void WriteHeader()
        {
            // Create and write the Protean header
            m_stream.Write(BitConverter.GetBytes(BinaryConstants.PROTEAN_MAGIC), 0, 4);
            m_stream.Write(BitConverter.GetBytes(BinaryConstants.VERSION_MAJOR << 16 | BinaryConstants.VERSION_MINOR), 0, 4);
            m_stream.Write(BitConverter.GetBytes((int)m_mode), 0, 4);
        }

        public void WriteString(string arg)
        {
            var bytes = Encoding.UTF8.GetBytes(arg);

            WriteInt(bytes.Length);
            WriteBytes(bytes, true);
        }

        private static int GetByteCountString(string arg)
        {
            return sizeof(Int32) + GetByteCountBytes(arg.Length, true);
        }

        public void WriteTimeSpan(TimeSpan arg)
        {
            WriteInt64((Int64)arg.TotalMilliseconds);
        }

        public static readonly long MaxDateTimeMillis = (Variant.MaxDateTime.Ticks - Variant.MinDateTime.Ticks) / 10000;

        public void WriteDateTime(DateTime arg)
        {
            if (arg == DateTime.MaxValue)
            {
                WriteInt64(MaxDateTimeMillis);
            }
            else
            {
                WriteInt64((arg.Ticks - Variant.MinDateTime.Ticks) / 10000);
            }
        }

        public void WriteByte(byte arg)
        {
            m_filter.WriteByte(arg);
        }

        public void WriteInt(Int32 arg)
        {
            WriteBytes(BitConverter.GetBytes(arg));
        }

        public void WriteUInt32(UInt32 arg)
        {
            WriteBytes(BitConverter.GetBytes(arg));
        }

        public void WriteInt64(Int64 arg)
        {
            WriteBytes(BitConverter.GetBytes(arg));
        }

        public void WriteUInt64(UInt64 arg)
        {
            WriteBytes(BitConverter.GetBytes(arg));
        }

        public void WriteFloat(float arg)
        {
            WriteBytes(BitConverter.GetBytes(arg));
        }

        public void WriteDouble(double arg)
        {
            WriteInt64(BitConverter.DoubleToInt64Bits(arg));
        }

        public void WriteBool(bool arg)
        {
            WriteInt(arg ? 1 : 0);
        }

        private void WriteBytes(byte[] arg)
        {
            m_filter.Write(arg, 0, arg.Length);
        }

        public void WriteBytes(byte[] bytes, bool writePadding)
        {
            WriteBytes(bytes);

            if (writePadding)
            {
                int residual = (4 - (bytes.Length % 4)) % 4;
                for (int i = 0; i < residual; ++i)
                {
                    WriteByte((byte)0);
                }
            }
        }

        private static int GetByteCountBytes(int byteCount, bool writePadding)
        {
            var residual = writePadding ? ((4 - (byteCount % 4)) % 4) : 0;
            return byteCount + residual;
        }

        public void WriteDataTable(DataTable arg)
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
            WriteInt(numCols);

            // Write number of rows
            WriteInt(numRows);

            // Write column types
            foreach (VariantBase.EnumType colType in colTypes)
            {
                WriteInt((int)colType);
            }

            // Write column names
            foreach (string colName in colNames)
            {
                WriteString(colName);
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

            // Write number of columns
            var writeCount = sizeof(int);

            // Write number of rows
            writeCount += sizeof(int);

            // Write column types
            writeCount += sizeof(int) * numCols;

            // Write column names
            foreach (var column in arg.Columns.Cast<DataColumn>())
            {
                writeCount += GetByteCountString(column.ColumnName);
            }

            // Write columns
            for (int i = 0; i < numCols; ++i)
            {
                var colWriteCounter = GetWriteCounterDelegate(VariantPrimitiveBase.TypeToEnum(arg.Columns[i].DataType));

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

        private void WriteArray(TypedArray arg)
        {
            var writer = GetWriterDelegate(arg.ElementType);

            WriteInt(arg.Count);
            WriteInt((int)arg.ElementType);

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
                    writer = o => WriteFloat((float) o);
                    break;
                case VariantBase.EnumType.Double:
                    writer = o => WriteDouble((double) o);
                    break;
                case VariantBase.EnumType.String:
                    writer = o => WriteString((string) o);
                    break;
                case VariantBase.EnumType.Boolean:
                    writer = o => WriteBool((bool) o);
                    break;
                case VariantBase.EnumType.Int32:
                    writer = o => WriteInt((int) o);
                    break;
                case VariantBase.EnumType.UInt32:
                    writer = o => WriteUInt32((uint) o);
                    break;
                case VariantBase.EnumType.Int64:
                    writer = o => WriteInt64((long) o);
                    break;
                case VariantBase.EnumType.UInt64:
                    writer = o => WriteUInt64((ulong) o);
                    break;
                case VariantBase.EnumType.Time:
                    writer = o => WriteTimeSpan((TimeSpan) o);
                    break;
                case VariantBase.EnumType.DateTime:
                    writer = o => WriteDateTime((DateTime) o);
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

        public void WriteVariant(Variant v)
        {
            VariantBase.EnumType type = v.Type;
            WriteInt((Int32)type);

            switch (type)
            {
                case VariantBase.EnumType.None:
                    break;
                case VariantBase.EnumType.String:
                case VariantBase.EnumType.Any:
                    WriteString(v.As<string>());
                    break;
                case VariantBase.EnumType.Float:
                    WriteFloat(v.As<float>());
                    break;
                case VariantBase.EnumType.Double:
                    WriteDouble(v.As<double>());
                    break;
                case VariantBase.EnumType.Int32:
                    WriteInt(v.As<Int32>());
                    break;
                case VariantBase.EnumType.UInt32:
                    WriteUInt32(v.As<UInt32>());
                    break;
                case VariantBase.EnumType.Int64:
                    WriteInt64(v.As<Int64>());
                    break;
                case VariantBase.EnumType.UInt64:
                    WriteUInt64(v.As<UInt64>());
                    break;
                case VariantBase.EnumType.Boolean:
                    WriteBool(v.As<bool>());
                    break;
                case VariantBase.EnumType.Time:
                    WriteTimeSpan(v.As<TimeSpan>());
                    break;
                case VariantBase.EnumType.DateTime:
                    WriteDateTime(v.As<DateTime>());
                    break;
                case VariantBase.EnumType.List:
                case VariantBase.EnumType.Tuple:
                    WriteInt(v.Count);
                    foreach (VariantItem item in v)
                    {
                        WriteVariant(item.Value);
                    }
                    break;
                case VariantBase.EnumType.Dictionary:
                case VariantBase.EnumType.Bag:
                    WriteInt(v.Count);
                    foreach (VariantItem item in v)
                    {
                        WriteString(item.Key);
                        WriteVariant(item.Value);
                    }
                    break;
                case VariantBase.EnumType.TimeSeries:
                    WriteInt(v.Count);
                    foreach (VariantItem item in v)
                    {
                        WriteDateTime(item.Time);
                        WriteVariant(item.Value);
                    }
                    break;
                case VariantBase.EnumType.Object:
                    IVariantObject o = v.AsObject();
                    WriteString(o.Class);
                    WriteInt(o.Version);
                    WriteVariant(o.Deflate());
                    break;
                case VariantBase.EnumType.Exception:
                    VariantExceptionInfo x = v.AsException();
                    WriteString(x.Class);
                    WriteString(x.Message);
                    WriteString(x.Source);
                    WriteString(x.Stack);
                    break;
                case VariantBase.EnumType.Buffer:
                    WriteInt(v.AsBuffer().Length);
                    WriteBytes(v.AsBuffer(), true);
                    break;
                case VariantBase.EnumType.DataTable:
                    WriteDataTable(v.AsDataTable());
                    break;
                case VariantBase.EnumType.Array:
                    WriteArray(v.AsArray());
                    break;
                default:
                    throw new VariantException("Case exhaustion: " + type.ToString());
            }
        }

        public static int GetByteCountVariant(Variant v)
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
