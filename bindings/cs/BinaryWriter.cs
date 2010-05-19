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
            Stream.Write(System.BitConverter.GetBytes((uint)Mode), 0, 4);
	    }

        private void Finish()
        {
            if ((Mode & BinaryMode.Compress) != 0)
            {
                Filter.Close();
            }
            Filter = null;
        }

	    private void WriteString(String arg)
	    {
		    byte[] bytes = System.Text.Encoding.ASCII.GetBytes(arg);
		    WriteInt32(bytes.Length);
		    WriteBytes(bytes);
	    }

        private void WriteTime(TimeSpan arg)
	    {
            WriteInt64((Int64)arg.TotalMilliseconds);
        }

        private void WriteDate(DateTime arg)
        {
            WriteInt32((Int32)(arg - Variant.MinDateTime).TotalDays);
        }

        private void WriteDateTime(DateTime arg)
        {
            WriteInt64((Int64)(arg - Variant.MinDateTime).TotalMilliseconds);
        }

	    private void WriteInt32(Int32 arg)
	    {
            Filter.Write(System.BitConverter.GetBytes(arg), 0, sizeof(Int32));
	    }

	    private void WriteUInt32(UInt32 arg)
	    {
            Filter.Write(System.BitConverter.GetBytes(arg), 0, sizeof(UInt32));
	    }

	    private void WriteInt64(Int64 arg)
	    {
            Filter.Write(System.BitConverter.GetBytes(arg), 0, sizeof(Int64));
	    }

	    private void WriteUInt64(UInt64 arg)
	    {
            Filter.Write(System.BitConverter.GetBytes(arg), 0, sizeof(UInt64));
	    }

        private void WriteFloat(float arg)
        {
            Filter.Write(System.BitConverter.GetBytes(arg), 0, sizeof(float));
        }

	    private void WriteDouble(double arg)
	    {
            Filter.Write(System.BitConverter.GetBytes(arg), 0, sizeof(double));
	    }

        private void WriteBytes(byte[] bytes)
        {
            Filter.Write(bytes, 0, bytes.Length);

            // add padding
		    int residual = (4 - (bytes.Length % 4)) % 4;
		    for(int i=0; i<residual; ++i)
		    {
                Filter.WriteByte(0);
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
            case Variant.EnumType.Float:
                WriteDouble(v.As<float>());
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
            case Variant.EnumType.TimeSeries:
                WriteUInt32((UInt32)v.Count);
                foreach (VariantItem item in v)
                {
                    WriteDateTime(item.Time);
                    WriteVariant(item.Value);
                }
                break;
		    default:
			    throw new VariantException("Case exhaustion: " + v.Type.ToString());
		    }
	    }
        private bool disposed;

        private System.IO.Stream Stream, Filter;
        private BinaryMode Mode;
    }
} // protean
