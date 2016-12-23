//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Data;
using System.IO;
using System.IO.Compression;

namespace Protean
{
	public class BinaryWriter : IDisposable
	{
		public BinaryWriter(Stream stream, BinaryMode mode)
		{
			m_mode = mode;

			m_stream = stream;
			if ((m_mode & BinaryMode.Compress) != 0)
			{
                DeflateStream ds = new DeflateStream(stream, CompressionMode.Compress, true);
                m_filter = new BufferedStream(ds, c_bufferSize);
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

        protected virtual void Dispose(bool disposing)
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

		protected void WriteHeader()
		{
			// Create and write the Protean header
			m_stream.Write(BitConverter.GetBytes(BinaryConstants.PROTEAN_MAGIC), 0, 4);
			m_stream.Write(BitConverter.GetBytes(BinaryConstants.VERSION_MAJOR << 16 | BinaryConstants.VERSION_MINOR), 0, 4);
			m_stream.Write(BitConverter.GetBytes((int)m_mode), 0, 4);
		}

		protected void Write(string arg)
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

		protected void Write(TimeSpan arg)
		{
			Write((Int64)arg.TotalMilliseconds);
		}

		private static readonly long MaxDateTimeMillis = (Variant.MaxDateTime.Ticks - Variant.MinDateTime.Ticks) / 10000;

		protected void Write(DateTime arg)
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

		protected void Write(Int32 arg)
		{
			Write(BitConverter.GetBytes(arg));
		}

		protected void Write(UInt32 arg)
		{
			Write(BitConverter.GetBytes(arg));
		}

		protected void Write(Int64 arg)
		{
			Write(BitConverter.GetBytes(arg));
		}

		protected void Write(UInt64 arg)
		{
			Write(BitConverter.GetBytes(arg));
		}

		protected void Write(float arg)
		{
			Write(BitConverter.GetBytes(arg));
		}

		protected void Write(double arg)
		{
			Write(BitConverter.DoubleToInt64Bits(arg));
		}

		private void Write(byte[] arg)
		{
			m_filter.Write(arg, 0, arg.Length);
		}

		protected void Write(bool arg)
		{
			Write((arg ? 1 : 0));
		}

		protected void Write(byte[] bytes, bool writePadding)
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

	    protected void Write(DataTable arg)
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
	            WriteDelegate colWriter;

                switch (colTypes[i])
                {
                    case VariantBase.EnumType.Float:
                        colWriter = o => Write((float)o);
                        break;
                    case VariantBase.EnumType.Double:
                        colWriter = o => Write((double)o);
                        break;
                    case VariantBase.EnumType.String:
                        colWriter = o => Write((string)o);
                        break;
                    case VariantBase.EnumType.Boolean:
                        colWriter = o => Write((bool)o);
                        break;
                    case VariantBase.EnumType.Int32:
                        colWriter = o => Write((Int32)o);
                        break;
                    case VariantBase.EnumType.UInt32:
                        colWriter = o => Write((UInt32)o);
                        break;
                    case VariantBase.EnumType.Int64:
                        colWriter = o => Write((Int64)o);
                        break;
                    case VariantBase.EnumType.UInt64:
                        colWriter = o => Write((UInt64)o);
                        break;
                    case VariantBase.EnumType.Time:
                        colWriter = o => Write((TimeSpan)o);
                        break;
                    case VariantBase.EnumType.DateTime:
                        colWriter = o => Write((DateTime)o);
                        break;
                    default:
                        throw new VariantException("Case exhaustion: " + colTypes[i]);
                }

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

		private void Write(TypedArray arg)
		{
			WriteDelegate writer;
			switch (arg.ElementType)
			{
				case VariantBase.EnumType.Float:
					writer = o => Write((float)o);
					break;
				case VariantBase.EnumType.Double:
					writer = o => Write((double)o);
					break;
				case VariantBase.EnumType.String:
					writer = o => Write((string)o);
					break;
				case VariantBase.EnumType.Boolean:
					writer = o => Write((bool)o);
					break;
				case VariantBase.EnumType.Int32:
					writer = o => Write((int)o);
					break;
				case VariantBase.EnumType.UInt32:
					writer = o => Write((uint)o);
					break;
				case VariantBase.EnumType.Int64:
					writer = o => Write((long)o);
					break;
				case VariantBase.EnumType.UInt64:
					writer = o => Write((ulong)o);
					break;
				case VariantBase.EnumType.Time:
					writer = o => Write((TimeSpan)o);
					break;
				case VariantBase.EnumType.DateTime:
					writer = o => Write((DateTime)o);
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

		private delegate void WriteDelegate(object arg);

		protected void WriteVariant(Variant v)
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

		private readonly Stream m_stream;
		private Stream m_filter;
		private readonly BinaryMode m_mode;
	    private const int c_bufferSize = 4*1024;
        private bool m_disposed;
	}
}

// Protean
