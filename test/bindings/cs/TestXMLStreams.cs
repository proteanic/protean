//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace protean.test
{
    using NUnit.Framework;
    using protean;

    [TestFixture]
    public class TestXMLStreams
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

            string xml = XMLWriter.ToString(v1, XMLMode.Indent);

            System.Console.WriteLine(xml);

            /*
            
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
             * 
             * */
        }
    }
}
