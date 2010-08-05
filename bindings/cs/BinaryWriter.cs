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

        public static byte[] ToBytes(Variant value, BinaryMode mode)
        {
            using (System.IO.MemoryStream ms = new System.IO.MemoryStream())
            {
                BinaryWriter writer = new BinaryWriter(ms, mode);
                writer.Write(value);
                return ms.GetBuffer();
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
            Finish();
        }

        private void WriteHeader()
        {
            // Create and write the Protean header
            Stream.Write(System.BitConverter.GetBytes(BinaryConstants.PROTEAN_MAGIC), 0, 4);
            Stream.Write(System.BitConverter.GetBytes(BinaryConstants.VERSION_MAJOR << 16 | BinaryConstants.VERSION_MINOR), 0, 4);
            Stream.Write(System.BitConverter.GetBytes((int)Mode), 0, 4);
        }

        private void Finish()
        {
            if ((Mode & BinaryMode.Compress) != 0)
            {
                Filter.Close();
            }
            Filter = null;
        }

        private void Write(string arg)
        {
            byte[] bytes = System.Text.Encoding.ASCII.GetBytes(arg);
            Write(bytes.Length);
            Write(bytes, true);
        }

        private void Write(TimeSpan arg)
        {
            Write((Int64)arg.TotalMilliseconds);
        }

        private void Write(DateTime arg)
        {
            Write((Int64)(arg - Variant.MinDateTime).TotalMilliseconds);
        }

        private void Write(byte arg)
        {
            Filter.WriteByte(arg);
        }

        private void Write(Int32 arg)
        {
            Write(System.BitConverter.GetBytes(arg));
        }

        private void Write(UInt32 arg)
        {
            Write(System.BitConverter.GetBytes(arg));
        }

        private void Write(Int64 arg)
        {
            Write(System.BitConverter.GetBytes(arg));
        }

        private void Write(UInt64 arg)
        {
            Write(System.BitConverter.GetBytes(arg));
        }

        private void Write(double arg)
        {
            Write(System.BitConverter.DoubleToInt64Bits(arg));
        }

        private void Write(byte[] arg)
        {
            Filter.Write(arg, 0, arg.Length);
        }

        private void Write(bool arg)
        {
            Write((int)(arg ? 1 : 0));
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

        private void WriteVariant(Variant v)
        {
            Variant.EnumType type = v.Type;
            Write((UInt32)type);

            switch (type)
            {
            case Variant.EnumType.None:
                break;
            case Variant.EnumType.String:
                Write(v.As<string>());
                break;
            case Variant.EnumType.Double:
                Write(v.As<double>());
                break;
            case Variant.EnumType.Int32:
                Write(v.As<Int32>());
                break;
            case Variant.EnumType.UInt32:
                Write(v.As<UInt32>());
                break;
            case Variant.EnumType.Int64:
                Write(v.As<Int64>());
                break;
            case Variant.EnumType.UInt64:
                Write(v.As<UInt64>());
                break;
            case Variant.EnumType.Boolean:
                Write(v.As<bool>());
                break;
            case Variant.EnumType.List:
            case Variant.EnumType.Tuple:
                Write(v.Count);
                foreach (VariantItem item in v)
                {
                    WriteVariant(item.Value);
                }
                break;
            case Variant.EnumType.Dictionary:
            case Variant.EnumType.Bag:
                Write(v.Count);
                foreach (VariantItem item in v)
                {
                    Write(item.Key);
                    WriteVariant(item.Value);
                }
                break;
            case Variant.EnumType.TimeSeries:
                Write(v.Count);
                foreach (VariantItem item in v)
                {
                    Write(item.Time);
                    WriteVariant(item.Value);
                }
                break;
            case Variant.EnumType.Object:
                VariantObjectBase o = v.AsObject();
                Write(o.Class);
                Write(o.Version);
                WriteVariant(o.Deflate());
                break;
            case Variant.EnumType.Buffer:
                Write(v.AsBuffer().Length);
                Write(v.AsBuffer(), true);
                break;
            default:
                throw new VariantException("Case exhaustion: " + type.ToString());
            }
        }

        private System.IO.Stream Stream, Filter;
        private BinaryMode Mode;
    }
} // protean
