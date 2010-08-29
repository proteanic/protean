//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Text;

using System.Data;

namespace protean {

    public class BinaryReader
    {
        public BinaryReader(System.IO.Stream stream, BinaryMode mode, IVariantObjectFactory factory)
        {
            m_mode = mode;
            m_factory = factory;
            m_stream = stream;
        }

        public BinaryReader(System.IO.Stream stream) :
            this(stream, BinaryMode.Default, null)
        { }
    
        public Variant Read()
        {
            ReadHeader();
            return ReadVariant();
        }

        public static Variant FromBytes(byte[] bytes, BinaryMode mode, IVariantObjectFactory factory)
        {
            using (System.IO.MemoryStream ms = new System.IO.MemoryStream(bytes))
            {
                BinaryReader reader = new BinaryReader(ms, mode, factory);
                return reader.Read();
            }
        }

        public static Variant FromBytes(byte[] bytes)
        {
            return FromBytes(bytes, BinaryMode.Default, null);
        }

        private void ReadHeader()
        {
            byte[] bytes = new byte[4];
            m_stream.Read(bytes, 0, 4);

            UInt32 magicNumber = System.BitConverter.ToUInt32(bytes, 0);
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

                m_filter = new System.IO.Compression.DeflateStream(m_stream, System.IO.Compression.CompressionMode.Decompress, true);
            }
            else
            {
                m_filter = m_stream;
            }
        }

        delegate object ReadDelegate();

        private Variant ReadVariant()
        {
            Variant.EnumType type = (Variant.EnumType)ReadUInt32();

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
                default:
                    throw new VariantException("Case exhaustion: " + type.ToString());
            }
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
        private byte[] ReadBytes(int length)
        {
            byte[] bytes = new byte[length];
            m_filter.Read(bytes, 0, length);
            return bytes;
        }
        private bool ReadBoolean()
        {
            return ReadInt32()!=0;
        }

        private string ReadString()
        {
            Int32 length = ReadInt32();
            byte[] bytes = ReadBytes(length, true);

            return System.Text.Encoding.ASCII.GetString(bytes, 0, length);
        }
        private float ReadFloat()
        {
            return System.BitConverter.ToSingle(ReadBytes(sizeof(float)), 0);
        }
        private double ReadDouble()
        {
            return System.BitConverter.Int64BitsToDouble(ReadInt64());
        }
        private Int32 ReadInt32()
        {
            return System.BitConverter.ToInt32(ReadBytes(sizeof(Int32)), 0);
        }
        private UInt32 ReadUInt32()
        {
            return System.BitConverter.ToUInt32(ReadBytes(sizeof(UInt32)), 0);
        }
        private Int64 ReadInt64()
        {
            return System.BitConverter.ToInt64(ReadBytes(sizeof(Int64)), 0);
        }
        private UInt64 ReadUInt64()
        {
            return System.BitConverter.ToUInt64(ReadBytes(sizeof(UInt64)), 0);
        }
        private TimeSpan ReadTime()
        {
            Int64 total_millis = ReadInt64();

            return new TimeSpan(total_millis * 10000);
        }
        private DateTime ReadDateTime()
        {
            Int64 total_millis = ReadInt64();

            return Variant.MinDateTime + new TimeSpan(total_millis * 10000);
        }
        private DataTable ReadDataTable()
        {
            int numCols = ReadInt32();
            int numRows = ReadInt32();
                    
            ReadDelegate[] colReaders = new ReadDelegate[numCols];
            Variant.EnumType[] colTypes = new Variant.EnumType[numCols];

            for (int i = 0; i < numCols; ++i)
            {
                colTypes[i] = (Variant.EnumType)ReadInt32();

                switch (colTypes[i])
                {
                    case VariantBase.EnumType.Float:
                        colReaders[i] = delegate() { return ReadFloat(); };
                        break;
                    case VariantBase.EnumType.Double:
                        colReaders[i] = delegate() { return ReadDouble(); };
                        break;
                    case VariantBase.EnumType.Boolean:
                        colReaders[i] = delegate() { return ReadBoolean(); };
                        break;
                    case VariantBase.EnumType.String:
                        colReaders[i] = delegate() { return ReadString(); };
                        break;
                    case VariantBase.EnumType.Int32:
                        colReaders[i] = delegate() { return ReadInt32(); };
                        break;
                    case VariantBase.EnumType.UInt32:
                        colReaders[i] = delegate() { return ReadUInt32(); };
                        break;
                    case VariantBase.EnumType.Int64:
                        colReaders[i] = delegate() { return ReadInt64(); };
                        break;
                    case VariantBase.EnumType.UInt64:
                        colReaders[i] = delegate() { return ReadUInt64(); };
                        break;
                    case VariantBase.EnumType.Time:
                        colReaders[i] = delegate() { return ReadTime(); };
                        break;
                    case VariantBase.EnumType.DateTime:
                        colReaders[i] = delegate() { return ReadDateTime(); };
                        break;
                }
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
            for (int i=0; i<numRows; ++i)
            {
                DataRow dr = dt.NewRow();
                for (int j = 0; j < numCols; ++j)
                {
                    dr[j] = colReaders[j]();
                }
                dt.Rows.Add(dr);
            }

            return dt;
        }

        private System.IO.Stream m_stream;
        private System.IO.Stream m_filter;
        private BinaryMode m_mode;
        private IVariantObjectFactory m_factory;
    }

} // protean
