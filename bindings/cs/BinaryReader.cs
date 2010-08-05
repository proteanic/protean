//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Text;

namespace protean {

    public class BinaryReader
    {
        public BinaryReader(System.IO.Stream stream)
        {
            Stream = stream;
        }
    
        public Variant Read()
        {
            ReadHeader();
            return ReadVariant();
        }

        public static Variant FromBytes(byte[] bytes)
        {
            using (System.IO.MemoryStream ms = new System.IO.MemoryStream(bytes))
            {
                BinaryReader reader = new BinaryReader(ms);
                return reader.Read();
            }
        }

        private void ReadHeader()
        {
            byte[] bytes = new byte[4];
            Stream.Read(bytes, 0, 4);

            UInt32 magicNumber = System.BitConverter.ToUInt32(bytes, 0);
            if (magicNumber != BinaryConstants.PROTEAN_MAGIC)
            {
                throw new VariantException("Invalid magic number");
            }

            // ignore version info for now
            Stream.Read(bytes, 0, 4);
    
            Stream.Read(bytes, 0, 4);
            BinaryMode writeMode = (BinaryMode)System.BitConverter.ToUInt32(bytes, 0);

            if ((writeMode & BinaryMode.Compress) != 0)
            {
                if ((writeMode & BinaryMode.ZlibHeader) != 0)
                {
                    throw new VariantException("Binary data appears to contain ZLIB header which is currently not supported in the protean.NET BinaryReader");
                }

                Filter = new System.IO.Compression.DeflateStream(Stream, System.IO.Compression.CompressionMode.Decompress, true);
            }
            else
            {
                Filter = Stream;
            }
        }

        private Variant ReadVariant()
        {
            Variant.EnumType type = (Variant.EnumType)ReadUInt32();

            switch (type)
            {
                case Variant.EnumType.None:
                    return new Variant(Variant.EnumType.None);
                case Variant.EnumType.String:
                    return new Variant(ReadString());
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

                    VariantObjectProxy proxy = new VariantObjectProxy(className);
                    proxy.Inflate(param, version);

                    return new Variant(proxy);
                }
                case Variant.EnumType.Buffer:
                {
                    int length = ReadInt32();
                    return new Variant(ReadBytes(length, true));
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
                    Filter.ReadByte();
                }
            }

            return bytes;
        }
        private byte[] ReadBytes(int length)
        {
            byte[] bytes = new byte[length];
            Filter.Read(bytes, 0, length);
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

        private System.IO.Stream Stream;
        private System.IO.Stream Filter;
    }

} // protean
