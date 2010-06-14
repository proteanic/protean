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

        void ReadHeader()
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

        Variant ReadVariant()
        {
            Variant.EnumType type = (Variant.EnumType)ReadUInt32();

            switch (type)
            {
                case Variant.EnumType.String:
                {
                    String value = ReadString();
                    return new Variant(value);
                }
                case Variant.EnumType.Float:
                {
                    float value = ReadFloat();
                    return new Variant(value);
                }
                case Variant.EnumType.Double:
                {
                    double value = ReadDouble();
                    return new Variant(value);
                }
                case Variant.EnumType.Int32:
                {
                    Int32 value = ReadInt32();
                    return new Variant(value);
                }
                case Variant.EnumType.UInt32:
                {
                    UInt32 value = ReadUInt32();
                    return new Variant(value);
                }
                case Variant.EnumType.Int64:
                {
                    Int64 value = ReadInt64();
                    return new Variant(value);
                }
                case Variant.EnumType.UInt64:
                {
                    UInt64 value = ReadUInt64();
                    return new Variant(value);
                }
                case Variant.EnumType.List:
                {
                    
                    Variant result = new Variant(Variant.EnumType.List);

                    UInt32 length = ReadUInt32();
                    for (UInt32 i=0; i<length; ++i)
                    {
                        result.Add(ReadVariant());
                    }

                    return result;
                }
                case Variant.EnumType.Dictionary:
                case Variant.EnumType.Bag:
                {
                    Variant result = new Variant(type);

                    UInt32 length = ReadUInt32();

                    for (UInt32 i=0; i<length; ++i)
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

                    UInt32 length = ReadUInt32();

                    for (UInt32 i = 0; i < length; ++i)
                    {
                        DateTime time = ReadDateTime();
                        Variant value = ReadVariant();

                        result.Add(time, value);
                    }

                    return result;
                }
                default:
                    throw new VariantException("Case exhaustion: " + type.ToString());
            }
        }

        byte[] ReadBytes(int length)
        {
            byte[] bytes = new byte[length];
            Filter.Read(bytes, 0, length);

            // Add padding
            int residual = (4 - (length % 4)) % 4;
            for(int i=0; i<residual; ++i)
            {
                Filter.ReadByte();
            }

            return bytes;
        }

        String ReadString()
        {
            Int32 length = ReadInt32();
            byte[] bytes = ReadBytes(length);

            return System.Text.Encoding.ASCII.GetString(bytes, 0, length);
        }
        float ReadFloat()
        {
            byte[] bytes = new byte[sizeof(float)];
            Filter.Read(bytes, 0, sizeof(float));
            return System.BitConverter.ToSingle(bytes, 0);
        }
        double ReadDouble()
        {
            byte[] bytes = new byte[sizeof(double)];
            Filter.Read(bytes, 0, sizeof(double));
            return System.BitConverter.ToDouble(bytes, 0);
        }
        Int32 ReadInt32()
        {
            byte[] bytes = new byte[sizeof(Int32)];
            Filter.Read(bytes, 0, sizeof(Int32));
            return System.BitConverter.ToInt32(bytes, 0);
        }
        UInt32 ReadUInt32()
        {
            byte[] bytes = new byte[sizeof(UInt32)];
            Filter.Read(bytes, 0, sizeof(UInt32));
            return System.BitConverter.ToUInt32(bytes, 0);
        }
        Int64 ReadInt64()
        {
            byte[] bytes = new byte[sizeof(Int64)];
            Filter.Read(bytes, 0, sizeof(Int64));
            return System.BitConverter.ToInt64(bytes, 0);
        }
        UInt64 ReadUInt64()
        {
            byte[] bytes = new byte[sizeof(UInt64)];
            Filter.Read(bytes, 0, sizeof(UInt64));
            return System.BitConverter.ToUInt64(bytes, 0);
        }
        DateTime ReadDate()
        {
            Int32 total_days = ReadInt32();

            return Variant.MinDateTime + new TimeSpan(total_days, 0, 0, 0);
        }
        TimeSpan ReadTime()
        {
            Int64 total_millis = ReadInt64();

            return new TimeSpan(total_millis * 10000);
        }
        DateTime ReadDateTime()
        {
            Int64 total_millis = ReadInt64();

            return Variant.MinDateTime + new TimeSpan(total_millis * 10000);
        }

        private System.IO.Stream Stream;
        private System.IO.Stream Filter;
    }

} // protean
