//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Text;

namespace protean {

    public class BinaryWriter
    {
        public enum EnumFlag { None = 0, Compress = 1 }

        private const UInt32 MagicNumber = 0x484913FF;
        private const UInt32 MajorVersion = 1;
        private const UInt32 MinorVersion = 0;

        public BinaryWriter(System.IO.Stream stream)
        {
            Stream = stream;
        }

        public void Write(Variant v)
        {
		    Write(v, EnumFlag.None);
        }

        void Write(Variant v, EnumFlag flags)
        {
		    WriteHeader(flags);
		    WriteVariant(v);
        }

	    private void WriteHeader(EnumFlag flags)
	    {
		    WriteUInt32((UInt32)MagicNumber);
            WriteUInt32((UInt32)((MajorVersion << 16) | MinorVersion));
            WriteUInt32((UInt32)0);
	    }

	    private void WriteString(String arg)
	    {
		    byte[] bytes = System.Text.Encoding.ASCII.GetBytes(arg);
		    WriteInt32(bytes.Length);
		    WriteBytes(bytes);
	    }

        private void WriteDate(DateTime arg)
	    {
        }

        private void WriteTime(TimeSpan arg)
	    {
        }

        private void WriteDateTime(DateTime arg)
        {
        }

	    private void WriteInt32(Int32 arg)
	    {
		    WriteBytes(System.BitConverter.GetBytes(arg));
	    }

	    private void WriteUInt32(UInt32 arg)
	    {
		    WriteBytes(System.BitConverter.GetBytes(arg));
	    }

	    private void WriteInt64(Int64 arg)
	    {
		    WriteBytes(System.BitConverter.GetBytes(arg));
	    }

	    private void WriteUInt64(UInt64 arg)
	    {
		    WriteBytes(System.BitConverter.GetBytes(arg));
	    }

	    private void WriteDouble(double arg)
	    {
		    WriteBytes(System.BitConverter.GetBytes(arg));
	    }

        private void WriteBytes(byte[] bytes)
        {
		    Stream.Write(bytes, 0, bytes.Length);
		    int residual = (4 - (bytes.Length % 4)) % 4;
		    for(int i=0; i<residual; ++i)
		    {
			    Stream.WriteByte(0);
		    }
        }

	    private void WriteVariant(Variant v)
	    {
		    WriteUInt32((UInt32)v.Type);

		    switch (v.Type)
		    {
		    case Variant.EnumType.String:
			    WriteString(v.As<String>());
			    break;
		    case Variant.EnumType.Double:
                WriteDouble(v.As<double>());
			    break;
            case Variant.EnumType.Int32:
                WriteInt32(v.As<Int32>());
                break;
            case Variant.EnumType.UInt32:
                WriteUInt32(v.As<UInt32>());
                break;
            case Variant.EnumType.Int64:
                WriteInt64(v.As<Int64>());
			    break;
		    case Variant.EnumType.UInt64:
                WriteUInt64(v.As<UInt64>());
			    break;
		    case Variant.EnumType.List:
			    WriteUInt32((UInt32)v.Count);
			    foreach (VariantItem item in v)
			    {
				    WriteVariant(item.Value);
			    }
			    break;
		    case Variant.EnumType.Dictionary:
            case Variant.EnumType.Bag:
			    WriteUInt32((UInt32)v.Count);
			    foreach (VariantItem item in v)
			    {
				    WriteString(item.Key);
				    WriteVariant(item.Value);
			    }
			    break;	
		    default:
			    throw new VariantException("Case exhaustion: " + v.Type.ToString());
		    }
	    }

        private System.IO.Stream Stream;
    }
} // protean
