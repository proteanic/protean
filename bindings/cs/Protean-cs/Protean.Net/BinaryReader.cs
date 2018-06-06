using System;
using System.Data;
using System.IO;
using System.Text;

namespace Protean {

    public class BinaryReader
    {
        public BinaryReader(Stream stream, BinaryMode mode, IVariantObjectFactory factory, bool requireSeekableReader)
        {
            m_mode = mode;
            m_factory = factory;
            m_stream = stream;
        	m_filter = stream;
        	m_requireSeekableReader = requireSeekableReader;
        }

        public BinaryReader(Stream stream, BinaryMode mode, IVariantObjectFactory factory) :
            this(stream, mode, factory, false)
        {}

		public BinaryReader(Stream stream, bool requireSeekableReader) :
            this(stream, BinaryMode.Default, null, requireSeekableReader)
        {}

		public BinaryReader(Stream stream) :
            this(stream, BinaryMode.Default, null, false)
        {}
    
        public Variant Read()
        {
            ReadHeader();
            return ReadVariant();
        }

        public static Variant FromBytes(byte[] bytes, BinaryMode mode, IVariantObjectFactory factory)
        {
            using (MemoryStream ms = new MemoryStream(bytes))
            {
                BinaryReader reader = new BinaryReader(ms, mode, factory);
                return reader.Read();
            }
        }

        unsafe public static Variant FromBytes(byte* bytes, long length)
        {
            return FromBytes(bytes, length, BinaryMode.Default, null);
        }

		unsafe public static Variant FromBytes(byte* bytes, long length, BinaryMode mode, IVariantObjectFactory factory)
		{
			using (UnmanagedMemoryStream ms = new UnmanagedMemoryStream(bytes, length))
			{
				BinaryReader reader = new BinaryReader(ms, mode, factory);
				return reader.Read();
			}
		}

		public static Variant FromBytes(byte[] bytes)
		{
			return FromBytes(bytes, BinaryMode.Default, null);
		}

    	protected long Position
    	{
			get { return m_filter.Position; }
    	}

		protected void Seek(long position, SeekOrigin origin)
		{
			m_filter.Seek(position, origin);
		}

    	protected void ReadHeader()
        {
            byte[] bytes = new byte[4];
            m_stream.Read(bytes, 0, 4);

            UInt32 magicNumber = BitConverter.ToUInt32(bytes, 0);
            if (magicNumber != BinaryConstants.PROTEAN_MAGIC)
            {
                throw new VariantException("Invalid magic number");
            }

            // ignore version info for now
            m_stream.Read(bytes, 0, 4);

            m_stream.Read(bytes, 0, 4);
            m_mode = (BinaryMode)System.BitConverter.ToUInt32(bytes, 0);

            if ((m_mode & BinaryMode.Compress) != 0)
            {
                if ((m_mode & BinaryMode.ZlibHeader) != 0)
                {
                    throw new VariantException("Binary data appears to contain ZLIB header which is currently not supported in the protean.NET BinaryReader");
                }

                System.IO.Compression.DeflateStream deflateStream = new System.IO.Compression.DeflateStream(m_stream, System.IO.Compression.CompressionMode.Decompress, true);

				if (m_requireSeekableReader)
				{
					MemoryStream deflatedStream = new MemoryStream();

					int value;

					while ((value = deflateStream.ReadByte()) != -1)
					{
						deflatedStream.WriteByte((byte)value);
					}

					m_filter = new MemoryStream(deflatedStream.ToArray(), 0, (int)deflatedStream.Length, false);
				}
				else
				{
					m_filter = deflateStream;
				}
            }
        }

        delegate object ReadDelegate();

        protected Variant ReadVariant()
        {
        	Variant.EnumType type = ReadType();

        	switch (type)
            {
                case Variant.EnumType.None:
                    return new Variant(Variant.EnumType.None);
                case Variant.EnumType.String:
                    return new Variant(ReadString());
                case Variant.EnumType.Any:
                    return new Variant(Variant.EnumType.Any, ReadString());
                case Variant.EnumType.Float:
                    return new Variant(ReadFloat());
                case Variant.EnumType.Double:
                    return new Variant(ReadDouble());
                case Variant.EnumType.Int32:
                    return new Variant(ReadInt32());
                case Variant.EnumType.UInt32:
                    return new Variant(ReadUInt32());
                case Variant.EnumType.Int64:
                    return new Variant(ReadInt64());
                case Variant.EnumType.UInt64:
                    return new Variant(ReadUInt64());
                case Variant.EnumType.Boolean:
                    return new Variant(ReadBoolean());
                case Variant.EnumType.Time:
                    if ((m_mode & BinaryMode.DateTimeAsTicks) == 0)
                    {
                        throw new VariantException("Binary data has DateTimeAsTicks mode disabled which is not supported in the protean.NET BinaryReader");
                    }
                    return new Variant(ReadTime());
                case Variant.EnumType.DateTime:
                    if ((m_mode & BinaryMode.DateTimeAsTicks) == 0)
                    {
                        throw new VariantException("Binary data has DateTimeAsTicks mode disabled which is not supported in the protean.NET BinaryReader");
                    }
                    return new Variant(ReadDateTime());
                case Variant.EnumType.Date:
                    throw new VariantException("Attempt to read Date variant which is no longer supported");
                case Variant.EnumType.Tuple:
                {
                    int length = ReadInt32();

                    Variant result = new Variant(Variant.EnumType.Tuple, length);
                    for (int i = 0; i < length; ++i)
                    {
                        result[i] = ReadVariant();
                    }
                    return result;
                }
                case Variant.EnumType.List:
                {
                    int length = ReadInt32();

                    Variant result = new Variant(type, length);
                    for (int i=0; i<length; ++i)
                    {
                        result.Add(ReadVariant());
                    }
                    return result;
                }
                case Variant.EnumType.Dictionary:
                case Variant.EnumType.Bag:
                {
                    int length = ReadInt32();

                    Variant result = new Variant(type);
                    for (int i=0; i<length; ++i)
                    {
                        String key = ReadString();
                        Variant value = ReadVariant();

                        result.Add(key, value);
                    }
                    return result;
                }
                case Variant.EnumType.TimeSeries:
                {
                    Variant result = new Variant(type);

                    int length = ReadInt32();

                    for (int i=0; i<length; ++i)
                    {
                        DateTime time = ReadDateTime();
                        Variant value = ReadVariant();

                        result.Add(time, value);
                    }

                    return result;
                }
                case Variant.EnumType.Object:
                {
                    string className = ReadString();
                    int version = ReadInt32();
                    Variant param = ReadVariant();


                    IVariantObject obj = null;
                    if (m_factory != null)
                    {
                        obj = m_factory.Create(className);

                        if (obj == null && (m_mode & BinaryMode.CreateProxy) == 0)
                        {
                            throw new VariantException("Object of class " + className + " is not regsistered in factory");
                        }
                    }

                    if (obj == null)
                    {
                        obj = new VariantObjectProxy(className);
                    }

                    obj.Inflate(param, version);

                    return new Variant(obj);
                }
                case Variant.EnumType.Exception:
                {
                    string xtype = ReadString();
                    string xmessage = ReadString();
                    string xsource = ReadString();
                    string xstack = ReadString();

                    return new Variant(new VariantExceptionInfo(xtype, xmessage, xsource, xstack));
                }
                case Variant.EnumType.Buffer:
                {
                    int length = ReadInt32();
                    return new Variant(ReadBytes(length, true));
                }
                case Variant.EnumType.DataTable:
                {
                     return new Variant(ReadDataTable());
                }
                case Variant.EnumType.Array:
                {
                    return new Variant(ReadArray());
                }
                default:
                    throw new VariantException("Case exhaustion: " + type.ToString());
            }
        }

    	protected Variant.EnumType ReadType()
    	{
    		return (Variant.EnumType)ReadUInt32();
    	}

    	private byte[] ReadBytes(int length, bool readPadding)
        {
            byte[] bytes = ReadBytes(length);

            if (readPadding)
            {
                int residual = (4 - (length % 4)) % 4;

                for (int i = 0; i < residual; ++i)
                {
                    m_filter.ReadByte();
                }
            }

            return bytes;
        }
		protected byte[] ReadBytes(int length)
        {
            byte[] bytes = new byte[length];
            m_filter.Read(bytes, 0, length);
            return bytes;
        }
		protected bool ReadBoolean()
        {
            return ReadInt32()!=0;
        }

		protected string ReadString()
        {
			Int32 length = ReadInt32();

			if (length > 0)
			{
				byte[] bytes = ReadBytes(length, true);

				unsafe
				{
					fixed (byte* ptr = bytes)
					{
					    return Encoding.UTF8.GetString(ptr, length);
					}
				}
			}
			else
			{
				return "";
			}
        }
		protected float ReadFloat()
        {
            return BitConverter.ToSingle(ReadBytes(sizeof(float)), 0);
        }
		protected double ReadDouble()
        {
            return BitConverter.Int64BitsToDouble(ReadInt64());
        }
		protected Int32 ReadInt32()
        {
            return BitConverter.ToInt32(ReadBytes(sizeof(Int32)), 0);
        }
		protected UInt32 ReadUInt32()
        {
            return BitConverter.ToUInt32(ReadBytes(sizeof(UInt32)), 0);
        }
		protected Int64 ReadInt64()
        {
            return BitConverter.ToInt64(ReadBytes(sizeof(Int64)), 0);
        }
		protected UInt64 ReadUInt64()
        {
            return BitConverter.ToUInt64(ReadBytes(sizeof(UInt64)), 0);
        }
		protected TimeSpan ReadTime()
        {
            Int64 totalMillis = ReadInt64();

            return new TimeSpan(totalMillis * 10000);
        }

        static readonly long MaxDateTimeMillis = (Variant.MaxDateTime.Ticks - Variant.MinDateTime.Ticks) / 10000;

		protected DateTime ReadDateTime()
        {
            long totalMillis = ReadInt64();

            if (totalMillis == MaxDateTimeMillis)
            {
                return DateTime.MaxValue;
            }
            else
            {
                return new DateTime(Variant.MinDateTime.Ticks + totalMillis * 10000);
            }
        }
		protected DataTable ReadDataTable()
        {
#if !DISABLE_DATATABLE
            int numCols = ReadInt32();
            int numRows = ReadInt32();

            Variant.EnumType[] colTypes = new Variant.EnumType[numCols];
		    for (int i = 0; i < numCols; ++i)
		    {
		        colTypes[i] = (Variant.EnumType)ReadInt32();
		    }

            string[] colNames = new string[numCols];
            for (int i = 0; i < numCols; ++i)
            {
                colNames[i] = ReadString();
            }

            DataTable dt = new DataTable();
            for (int i = 0; i < numCols; ++i)
            {
                dt.Columns.Add(new DataColumn(colNames[i], VariantPrimitiveBase.EnumToType(colTypes[i])));
            };

            // Write number of rows
		    for (int col = 0; col < numCols; ++col)
		    {
		        ReadDelegate colReader;
                switch (colTypes[col])
		        {
		            case VariantBase.EnumType.Float:
                        colReader = () => ReadFloat();
		                break;
		            case VariantBase.EnumType.Double:
                        colReader = () => ReadDouble();
		                break;
		            case VariantBase.EnumType.Boolean:
                        colReader = () => ReadBoolean();
		                break;
		            case VariantBase.EnumType.String:
                        colReader = () => ReadString();
		                break;
		            case VariantBase.EnumType.Int32:
                        colReader = () => ReadInt32();
		                break;
		            case VariantBase.EnumType.UInt32:
                        colReader = () => ReadUInt32();
		                break;
		            case VariantBase.EnumType.Int64:
                        colReader = () => ReadInt64();
		                break;
		            case VariantBase.EnumType.UInt64:
                        colReader = () => ReadUInt64();
		                break;
		            case VariantBase.EnumType.Time:
                        colReader = () => ReadTime();
		                break;
		            case VariantBase.EnumType.DateTime:
                        colReader = () => ReadDateTime();
		                break;
		            default:
                        throw new VariantException("Case exhaustion: " + colTypes[col]);
		        }

		        for (int row = 0; row < numRows; ++row)
		        {
		            DataRow dr;
		            if (col == 0)
		            {
                        dr = dt.NewRow();
                        dt.Rows.Add(dr);
		            }
		            else
		            {
		                dr = dt.Rows[row];
		            }

                    dr[col] = colReader();
		        }
		    }
        
            return dt;
#else
            throw new NotSupportedException("Datatables not supported on this platform.");
#endif
        }

        private TypedArray ReadArray()
        {
            int size = ReadInt32();
            Variant.EnumType elementType = (Variant.EnumType)ReadInt32();

            TypedArray array = new TypedArray(elementType, size);

            ReadDelegate reader = null;
            switch (elementType)
            {
                case VariantBase.EnumType.Float:
                    reader = () => ReadFloat();
                    break;
                case VariantBase.EnumType.Double:
                    reader = () => ReadDouble();
                    break;
                case VariantBase.EnumType.Boolean:
                    reader = () => ReadBoolean();
                    break;
                case VariantBase.EnumType.String:
                    reader = () => ReadString();
                    break;
                case VariantBase.EnumType.Int32:
                    reader = () => ReadInt32();
                    break;
                case VariantBase.EnumType.UInt32:
                    reader = () => ReadUInt32();
                    break;
                case VariantBase.EnumType.Int64:
                    reader = () => ReadInt64();
                    break;
                case VariantBase.EnumType.UInt64:
                    reader = () => ReadUInt64();
                    break;
                case VariantBase.EnumType.Time:
                    reader = () => ReadTime();
                    break;
                case VariantBase.EnumType.DateTime:
                    reader = () => ReadDateTime();
                    break;
                default:
                    throw new VariantException("Case exhaustion: " + array.ElementType); 
            }

            for (int i = 0; i < size; ++i)
            {
                array[i] = reader();
            }

            return array;
        }

		protected void SkipVariant()
		{
			Variant.EnumType type = ReadType();

			switch (type)
			{
				case Variant.EnumType.None:
					break;
				case Variant.EnumType.String:
					SkipString();
					break;
				case Variant.EnumType.Any:
					SkipString();
					break;
				case Variant.EnumType.Float:
					SkipFloat();
					break;
				case Variant.EnumType.Double:
					SkipDouble();
					break;
				case Variant.EnumType.Int32:
					SkipInt32();
					break;
				case Variant.EnumType.UInt32:
					SkipUInt32();
					break;
				case Variant.EnumType.Int64:
					SkipInt64();
					break;
				case Variant.EnumType.UInt64:
					SkipUInt64();
					break;
				case Variant.EnumType.Boolean:
					SkipBoolean();
					break;
				case Variant.EnumType.Time:
					if ((m_mode & BinaryMode.DateTimeAsTicks) == 0)
					{
						throw new VariantException("Binary data has DateTimeAsTicks mode disabled which is not supported in the protean.NET BinaryReader");
					}
					SkipTime();
					break;
				case Variant.EnumType.DateTime:
					if ((m_mode & BinaryMode.DateTimeAsTicks) == 0)
					{
						throw new VariantException("Binary data has DateTimeAsTicks mode disabled which is not supported in the protean.NET BinaryReader");
					}
					SkipDateTime();
					break;
				case Variant.EnumType.Date:
					throw new VariantException("Attempt to read Date variant which is no longer supported");
				case Variant.EnumType.Tuple:
				case Variant.EnumType.List:
					{
						int length = ReadInt32();

						for (int i = 0; i < length; ++i)
						{
							SkipVariant();
						}
					}
					break;
				case Variant.EnumType.Dictionary:
				case Variant.EnumType.Bag:
					{
						int length = ReadInt32();

						for (int i = 0; i < length; ++i)
						{
							SkipString();
							SkipVariant();
						}
					}
					break;
				case Variant.EnumType.TimeSeries:
					{
						int length = ReadInt32();

						for (int i = 0; i < length; ++i)
						{
							SkipDateTime();
							SkipVariant();
						}
					}
					break;
				case Variant.EnumType.Object:
					{
						SkipString();
						SkipInt32();
						SkipVariant();
					}
					break;
				case Variant.EnumType.Exception:
					{
						SkipString();
						SkipString();
						SkipString();
						SkipString();
					}
					break;
				case Variant.EnumType.Buffer:
					{
						int length = ReadInt32();
						SkipBytes(length, true);
					}
					break;
				case Variant.EnumType.DataTable:
					{
						SkipDataTable();
					}
					break;
				case Variant.EnumType.Array:
					{
						SkipArray();
					}
					break;
				default:
					throw new VariantException("Case exhaustion: " + type.ToString());
			}
		}

		private void SkipBytes(int length, bool readPadding)
		{
			m_filter.Position += length;

			if (readPadding)
			{
				m_filter.Position += (4 - (length % 4)) % 4;
			}
		}

		private void SkipBytes(int length)
		{
			m_filter.Seek(length, SeekOrigin.Current);
		}

		private void SkipDataTable()
		{
			int numCols = ReadInt32();
			int numRows = ReadInt32();

			Action[] columnSkippers = new Action[numCols];
			Variant.EnumType[] colTypes = new Variant.EnumType[numCols];

			for (int i = 0; i < numCols; ++i)
			{
				colTypes[i] = (Variant.EnumType)ReadInt32();

				switch (colTypes[i])
				{
					case VariantBase.EnumType.Float:
						columnSkippers[i] = SkipFloat;
						break;
					case VariantBase.EnumType.Double:
						columnSkippers[i] = SkipDouble;
						break;
					case VariantBase.EnumType.Boolean:
						columnSkippers[i] = SkipBoolean;
						break;
					case VariantBase.EnumType.String:
						columnSkippers[i] = SkipString;
						break;
					case VariantBase.EnumType.Int32:
						columnSkippers[i] = SkipInt32;
						break;
					case VariantBase.EnumType.UInt32:
						columnSkippers[i] = SkipUInt32;
						break;
					case VariantBase.EnumType.Int64:
						columnSkippers[i] = SkipInt64;
						break;
					case VariantBase.EnumType.UInt64:
						columnSkippers[i] = SkipUInt64;
						break;
					case VariantBase.EnumType.Time:
						columnSkippers[i] = SkipTime;
						break;
					case VariantBase.EnumType.DateTime:
						columnSkippers[i] = SkipDateTime;
						break;
					default:
						throw new VariantException("Case exhaustion: " + colTypes[i]);
				}
			}

			for (int i = 0; i < numCols; ++i)
			{
				SkipString();
			}

			for (int i = 0; i < numRows; ++i)
			{
				for (int j = 0; j < numCols; ++j)
				{
					columnSkippers[j]();
				}
			}
		}

		private void SkipArray()
		{
			int size = ReadInt32();
			Variant.EnumType elementType = (Variant.EnumType)ReadInt32();

			Action skipper;

			switch (elementType)
			{
				case VariantBase.EnumType.Float:
					skipper = SkipFloat;
					break;
				case VariantBase.EnumType.Double:
					skipper = SkipDouble;
					break;
				case VariantBase.EnumType.Boolean:
					skipper = SkipBoolean;
					break;
				case VariantBase.EnumType.String:
					skipper = SkipString;
					break;
				case VariantBase.EnumType.Int32:
					skipper = SkipInt32;
					break;
				case VariantBase.EnumType.UInt32:
					skipper = SkipUInt32;
					break;
				case VariantBase.EnumType.Int64:
					skipper = SkipInt64;
					break;
				case VariantBase.EnumType.UInt64:
					skipper = SkipUInt64;
					break;
				case VariantBase.EnumType.Time:
					skipper = SkipTime;
					break;
				case VariantBase.EnumType.DateTime:
					skipper = SkipDateTime;
					break;
				default:
					throw new VariantException("Case exhaustion: " + elementType);
			}

			for (int i = 0; i < size; ++i)
			{
				skipper();
			}
		}

		private void SkipDateTime()
		{
			SkipInt64();
		}

		private void SkipTime()
		{
			SkipInt64();
		}

		private void SkipBoolean()
		{
			SkipInt32();
		}

		private void SkipUInt64()
		{
			SkipBytes(sizeof(UInt64));
		}

		private void SkipInt64()
		{
			SkipBytes(sizeof(Int64));
		}

		private void SkipUInt32()
		{
			SkipBytes(sizeof(UInt32));
		}

		private void SkipInt32()
		{
			SkipBytes(sizeof(Int32));
		}

		private void SkipDouble()
		{
			SkipInt64();
		}

		private void SkipFloat()
		{
			SkipBytes(sizeof(float));
		}

    	private void SkipString()
    	{
			SkipBytes(ReadInt32(), true);
    	}

    	private readonly System.IO.Stream m_stream;
        private System.IO.Stream m_filter;
        private BinaryMode m_mode;
        private readonly IVariantObjectFactory m_factory;
    	private readonly bool m_requireSeekableReader;
    }

} // Protean
