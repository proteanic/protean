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
        private const UInt32 MagicNumber = 0x484913FF;
        private const UInt32 MajorVersion = 1;
        private const UInt32 MinorVersion = 0;

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
            UInt32 magicNumber = ReadUInt32();
            if (magicNumber != MagicNumber)
            {
                throw new VariantException("Invalid magic number");
            }

            // TODO
            ReadUInt32();
            ReadUInt32();
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
		        break;
		        }
	        case Variant.EnumType.Double:
		        {
		        double value = ReadDouble();
		        return new Variant(value);
		        break;
		        }
	        case Variant.EnumType.Int64:
		        {
		        Int64 value = ReadInt64();
		        return new Variant(value);
		        break;
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
    		
		        break;
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
		        break;
		        }
	        default:
		        throw new VariantException("Case exhaustion: " + type.ToString());
	        }
        }

        byte[] ReadBytes(int length)
        {
	        byte[] bytes = new byte[length];
	        Stream.Read(bytes, 0, length);

            // Add padding
	        int residual = (4 - (length % 4)) % 4;
	        for(int i=0; i<residual; ++i)
	        {
		        Stream.ReadByte();
	        }

            return bytes;
        }

        String ReadString()
        {
	        Int32 length = ReadInt32();
            byte[] bytes = ReadBytes(length);

	        return System.Text.Encoding.ASCII.GetString(bytes, 0, length);
        }

        double ReadDouble()
        {
	        byte[] bytes = ReadBytes(sizeof(Double));
	        return System.BitConverter.ToDouble(bytes, 0);
        }
        Int32 ReadInt32()
        {
	        byte[] bytes = ReadBytes(sizeof(Int32));
	        return System.BitConverter.ToInt32(bytes, 0);
        }
        UInt32 ReadUInt32()
        {
	        byte[] bytes = ReadBytes(sizeof(UInt32));
	        return System.BitConverter.ToUInt32(bytes, 0);
        }
        Int64 ReadInt64()
        {
	        byte[] bytes = ReadBytes(sizeof(Int64));
	        return System.BitConverter.ToInt64(bytes, 0);
        }
        DateTime ReadDateTime(bool ignoreTime)
        {
	        return new DateTime(1400, 1, 1);
        }

        private System.IO.Stream Stream;
    }

} // protean
