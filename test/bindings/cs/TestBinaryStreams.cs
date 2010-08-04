using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace protean.test
{
    using NUnit.Framework;
    using protean;

    [TestFixture]
    public class TestBinaryStreams
    {
        [Test]
        public void TestPrimitives()
        {
            Int32 argInt32 = -1;
            UInt32 argUInt32 = 0xffffffff;
            Int64 argInt64 = -1;
            UInt64 argUInt64 = 0xffffffffffffffff;
            bool argBoolean = true;
            double argDouble = double.MaxValue;
            string argString = "test string";

            Variant v1 = new Variant(Variant.EnumType.Dictionary);
            v1.Add("Int32", new Variant(argInt32));
            v1.Add("UInt32", new Variant(argUInt32));
            v1.Add("Int64", new Variant(argInt64));
            v1.Add("UInt64", new Variant(argUInt64));
            v1.Add("Boolean", new Variant(argBoolean));
            v1.Add("Double", new Variant(argDouble));
            v1.Add("String", new Variant(argString));
            v1.Add("None", new Variant(Variant.EnumType.None));

            byte[] bytesCompressed = BinaryWriter.ToBytes(v1, BinaryMode.Compress);
            byte[] bytesUncompressed = BinaryWriter.ToBytes(v1);

            Variant v2 = BinaryReader.FromBytes(bytesCompressed);

            Assert.AreEqual(v2.Type, Variant.EnumType.Dictionary);
            Assert.AreEqual(v2["String"].Type, Variant.EnumType.String);
            Assert.AreEqual(v2["Int32"].Type, Variant.EnumType.Int32);
            Assert.AreEqual(v2["UInt32"].Type, Variant.EnumType.UInt32);
            Assert.AreEqual(v2["Int64"].Type, Variant.EnumType.Int64);
            Assert.AreEqual(v2["UInt64"].Type, Variant.EnumType.UInt64);
            Assert.AreEqual(v2["Boolean"].Type, Variant.EnumType.Boolean);
            Assert.AreEqual(v2["Double"].Type, Variant.EnumType.Double);
            Assert.AreEqual(v2["None"].Type, Variant.EnumType.None);

            Assert.AreEqual(v2["String"].As<string>(), argString);
            Assert.AreEqual(v2["Int32"].As<Int32>(), argInt32);
            Assert.AreEqual(v2["UInt32"].As<UInt32>(), argUInt32);
            Assert.AreEqual(v2["Int64"].As<Int64>(), argInt64);
            Assert.AreEqual(v2["UInt64"].As<UInt64>(), argUInt64);
            Assert.AreEqual(v2["Boolean"].As<Boolean>(), argBoolean);
            Assert.AreEqual(v2["Double"].As<Double>(), argDouble);

            Variant v3 = BinaryReader.FromBytes(bytesUncompressed);

            Assert.IsTrue(v2.Equals(v3));
        }

        [Test]
        // Test compatibility with CLR variant
        public void TestCompatibility()
        {
            Int32 argInt32 = -1;
            UInt32 argUInt32 = 0xffffffff;
            Int64 argInt64 = -1;
            UInt64 argUInt64 = 0xffffffffffffffff;
            bool argBoolean = true;
            double argDouble = double.MaxValue;
            string argString = "test string";

            Variant v1 = new Variant(Variant.EnumType.Dictionary);
            v1.Add("Int32", new Variant(argInt32));
            v1.Add("UInt32", new Variant(argUInt32));
            v1.Add("Int64", new Variant(argInt64));
            v1.Add("UInt64", new Variant(argUInt64));
            v1.Add("Boolean", new Variant(argBoolean));
            v1.Add("Double", new Variant(argDouble));
            v1.Add("String", new Variant(argString));
            v1.Add("None", new Variant(Variant.EnumType.None));

            byte[] bytesCompressed = BinaryWriter.ToBytes(v1, BinaryMode.Compress);
            byte[] bytesUncompressed = BinaryWriter.ToBytes(v1);

            protean.clr.Variant v2 = protean.clr.BinaryReader.FromBytes(bytesCompressed);

            Assert.AreEqual(v2.Type, protean.clr.Variant.EnumType.Dictionary);
            Assert.AreEqual(v2["String"].Type, protean.clr.Variant.EnumType.String);
            Assert.AreEqual(v2["Int32"].Type, protean.clr.Variant.EnumType.Int32);
            Assert.AreEqual(v2["UInt32"].Type, protean.clr.Variant.EnumType.UInt32);
            Assert.AreEqual(v2["Int64"].Type, protean.clr.Variant.EnumType.Int64);
            Assert.AreEqual(v2["UInt64"].Type, protean.clr.Variant.EnumType.UInt64);
            Assert.AreEqual(v2["Boolean"].Type, protean.clr.Variant.EnumType.Boolean);
            Assert.AreEqual(v2["Double"].Type, protean.clr.Variant.EnumType.Double);
            Assert.AreEqual(v2["None"].Type, protean.clr.Variant.EnumType.None);

            Assert.AreEqual(v2["String"].AsString(), argString);
            Assert.AreEqual(v2["Int32"].AsInt32(), argInt32);
            Assert.AreEqual(v2["UInt32"].AsUInt32(), argUInt32);
            Assert.AreEqual(v2["Int64"].AsInt64(), argInt64);
            Assert.AreEqual(v2["UInt64"].AsUInt64(), argUInt64);
            Assert.AreEqual(v2["Boolean"].AsBoolean(), argBoolean);
            Assert.AreEqual(v2["Double"].AsDouble(), argDouble);

            protean.clr.Variant v3 = protean.clr.BinaryReader.FromBytes(bytesUncompressed);

            Assert.IsTrue(v2.Equals(v3));
        }
    }
}
