//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;

namespace Protean.Test
{
    using NUnit.Framework;
    using Protean;

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
            double argDouble = 2.0;
            string argString = "test string";
            DateTime argDateTime = new DateTime(2010, 1, 2, 3, 4, 5, 6);
            TimeSpan argTime = new TimeSpan(0, 1, 2, 3, 4);

            Variant v1 = new Variant(Variant.EnumType.Dictionary);
            v1.Add("Int32", new Variant(argInt32));
            v1.Add("UInt32", new Variant(argUInt32));
            v1.Add("Int64", new Variant(argInt64));
            v1.Add("UInt64", new Variant(argUInt64));
            v1.Add("Boolean", new Variant(argBoolean));
            v1.Add("Double", new Variant(argDouble));
            v1.Add("String", new Variant(argString));
            v1.Add("DateTime", new Variant(argDateTime));
            v1.Add("Time", new Variant(argTime));
            v1.Add("None", new Variant(Variant.EnumType.None));

            string xml = XmlWriter.ToString(v1);
            Variant v2 = XmlReader.FromString(xml);

            Assert.IsTrue(v1.Equals(v2));
        }

        [Test]
        public void TestUntypedXML()
        {
            string xml =
                @"<?xml version=""1.0"" encoding=""utf-8""?>
                    <element1>
                        <element2>
	                        <element3>value1</element3>
                        </element2>
                        <element4 attr1=""value"">
                            <element5>
	                            <element6>value2</element6>
                                <element7/>
                            </element5>
                        </element4>
                        <element8 attr2=""value""/>
                    </element1>";

            Variant v = XmlReader.FromString(xml);

            Assert.AreEqual(Variant.EnumType.Bag, v.Type);
            Assert.AreEqual(Variant.EnumType.Bag, v["element2"].Type);
            Assert.AreEqual(Variant.EnumType.Any, v["element2"]["element3"].Type);
            Assert.AreEqual(Variant.EnumType.Bag, v["element4"].Type);
            Assert.AreEqual(Variant.EnumType.Any, v["element4"]["attr1"].Type);
            Assert.AreEqual(Variant.EnumType.Bag, v["element4"]["element5"].Type);
            Assert.AreEqual(Variant.EnumType.Any, v["element4"]["element5"]["element6"].Type);
            Assert.AreEqual(Variant.EnumType.Any, v["element4"]["element5"]["element7"].Type);
            Assert.AreEqual(Variant.EnumType.Bag, v["element8"].Type);
            Assert.AreEqual(Variant.EnumType.Any, v["element8"]["attr2"].Type);
        }

        class TestObject1 : IVariantObject
        {
            public TestObject1(string value)
            {
                m_value = value;
            }

            public TestObject1() :
                this("default")
            { }

            public string Class { get { return "TestObject1"; } }

            public int Version { get { return 1; } }

            public Variant Deflate()
            {
                Variant result = new Variant(Variant.EnumType.Dictionary);
                result.Add("value", new Variant(m_value));
                return result;
            }

            public void Inflate(Variant param, int version)
            {
                m_value = param["value"].As<string>();
            }

            private string m_value;
        }

        [Test]
        public void TestObject()
        {
            TestObject1 o1 = new TestObject1("some value");

            Variant v1 = new Variant(o1);

            string xml = XmlWriter.ToString(v1);
            Variant v2 = XmlReader.FromString(xml);

            Assert.AreEqual(v2.Type, Variant.EnumType.Object);

            TestObject1 o2 = v2.AsObject<TestObject1>();

            Assert.AreEqual(o1.Class, o2.Class);
            Assert.AreEqual(o1.Version, o2.Version);
            Assert.IsTrue(o1.Deflate().Equals(o2.Deflate()));
        }

        [Test]
        public void TestTimeSeries()
        {
            Variant v1 = new Variant(Variant.EnumType.TimeSeries);
            v1.Add(new DateTime(2010, 1, 2, 3, 4, 5, 6), new Variant("value1"));
            v1.Add(new DateTime(2010, 1, 3, 3, 4, 5, 6), new Variant("value2"));

            string xml = XmlWriter.ToString(v1);
            Variant v2 = XmlReader.FromString(xml);

            Assert.AreEqual(v2.Type, Variant.EnumType.TimeSeries);
            Assert.IsTrue(v1.Equals(v2));
        }

        [Test]
        public void TestList()
        {
            Variant v1 = new Variant(Variant.EnumType.List);
            v1.Add(new Variant("value1"));
            v1.Add(new Variant(1.0));

            string xml = XmlWriter.ToString(v1);
            Variant v2 = XmlReader.FromString(xml);

            Assert.AreEqual(v2.Type, Variant.EnumType.List);
            Assert.IsTrue(v1.Equals(v2));
        }

        [Test]
        public void TestBag()
        {
            Variant v1 = new Variant(Variant.EnumType.Bag);
            v1.Add("key1", new Variant("value1"));
            v1.Add("key2", new Variant(1.0));

            string xml = XmlWriter.ToString(v1);
            Variant v2 = XmlReader.FromString(xml);

            Assert.AreEqual(v2.Type, Variant.EnumType.Bag);
            Assert.IsTrue(v1.Equals(v2));
        }

        [Test]
        public void TestBuffer()
        {
            byte[] bytes = new byte[] { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 };

            Variant v1 = new Variant(bytes);

            string xml = XmlWriter.ToString(v1);
            Variant v2 = XmlReader.FromString(xml);

            Assert.AreEqual(v2.Type, Variant.EnumType.Buffer);
            Assert.IsTrue(v1.Equals(v2));
        }

        [Test]
        public void TestTuple()
        {
            Variant v1 = new Variant(Variant.EnumType.Tuple, 2);
            v1[0] = new Variant("value");
            v1[1] = new Variant(1.0);

            string xml = XmlWriter.ToString(v1);
            Variant v2 = XmlReader.FromString(xml);

            Assert.AreEqual(v2.Type, Variant.EnumType.Tuple);
            Assert.IsTrue(v1.Equals(v2));
        }

        [Test]
        public void TestXmlValidation()
        {
            string xml =
                "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" +
                "<Variant aBoolean=\"true\" aDate=\"2007-01-03\" aDateTime=\"2007-01-03T10:30:00\" aFloat=\"0.5\" aDouble=\"3.9\" aInteger=\"1\" aUnsigned=\"2\" aInt32=\"3\" aUInt32=\"4\" aInt64=\"5\" aUInt64=\"6\" aString=\"Element\" aTime=\"10:30:00\">\n" +
                "  <Boolean>true</Boolean>\n" +
                "  <Date>2007-01-03</Date>\n" +
                "  <DateTime>2007-01-03T10:30:00</DateTime>\n" +
                "  <Float>0.5</Float>\n" +
                "  <Double>1</Double>\n" +
                "  <Integer>1</Integer>\n" +
                "  <Unsigned>2</Unsigned>\n" +
                "  <Int32>3</Int32>\n" +
                "  <UInt32>4</UInt32>\n" +
                "  <Int64>5</Int64>\n" +
                "  <UInt64>6</UInt64>\n" +
                "  <String>Element</String>\n" +
                "  <Time>10:30:00</Time>\n" +
                "</Variant>";

            string xsd =
                "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" +
                "<xsd:schema xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\n" +
                "  <xsd:element name=\"Variant\">\n" +
                "    <xsd:complexType mixed=\"true\">\n" +
                "      <xsd:all>\n" +
                "        <xsd:element name=\"String\"   type=\"xsd:string\"/>\n" +
                "        <xsd:element name=\"Boolean\"  type=\"xsd:boolean\"/>\n" +
                "        <xsd:element name=\"Integer\"  type=\"xsd:integer\"/>\n" +
                "        <xsd:element name=\"Unsigned\" type=\"xsd:positiveInteger\"/>\n" +
                "        <xsd:element name=\"Int32\"    type=\"xsd:int\"/>\n" +
                "        <xsd:element name=\"UInt32\"   type=\"xsd:unsignedInt\"/>\n" +
                "        <xsd:element name=\"Int64\"    type=\"xsd:long\"/>\n" +
                "        <xsd:element name=\"UInt64\"   type=\"xsd:unsignedLong\"/>\n" +
                "        <xsd:element name=\"Float\"    type=\"xsd:float\"/>\n" +
                "        <xsd:element name=\"Double\"   type=\"xsd:double\"/>\n" +
                "        <xsd:element name=\"Date\"     type=\"xsd:date\"/>\n" +
                "        <xsd:element name=\"Time\"     type=\"xsd:time\"/>\n" +
                "        <xsd:element name=\"DateTime\" type=\"xsd:dateTime\"/>\n" +
                "      </xsd:all>\n" +
                "      <xsd:attribute name=\"variant\"   type=\"xsd:string\" use=\"optional\" default=\"Dictionary\"/>\n" +
                "      <xsd:attribute name=\"aString\"   type=\"xsd:string\"/>\n" +
                "      <xsd:attribute name=\"aBoolean\"  type=\"xsd:boolean\"/>\n" +
                "      <xsd:attribute name=\"aInteger\"  type=\"xsd:integer\"/>\n" +
                "      <xsd:attribute name=\"aUnsigned\" type=\"xsd:positiveInteger\"/>\n" +
                "      <xsd:attribute name=\"aInt32\"    type=\"xsd:int\"/>\n" +
                "      <xsd:attribute name=\"aUInt32\"   type=\"xsd:unsignedInt\"/>\n" +
                "      <xsd:attribute name=\"aInt64\"    type=\"xsd:long\"/>\n" +
                "      <xsd:attribute name=\"aUInt64\"   type=\"xsd:unsignedLong\"/>\n" +
                "      <xsd:attribute name=\"aFloat\"    type=\"xsd:float\"/>\n" +
                "      <xsd:attribute name=\"aDouble\"   type=\"xsd:double\"/>\n" +
                "      <xsd:attribute name=\"aDate\"     type=\"xsd:date\"/>\n" +
                "      <xsd:attribute name=\"aTime\"     type=\"xsd:time\"/>\n" +
                "      <xsd:attribute name=\"aDateTime\" type=\"xsd:dateTime\"/>\n" +
                "    </xsd:complexType>\n" +
                "  </xsd:element>\n" +
                "</xsd:schema>\n";

            Variant v1;
            using ( System.IO.StringReader xmlStream = new System.IO.StringReader(xml), 
                                            xsdStream = new System.IO.StringReader(xsd) )
            {
                v1 = XmlReader.Create(stream: xmlStream, xsdStream: xsdStream);
            }

            Assert.AreEqual(v1.Type, Variant.EnumType.Dictionary);

            // check attribute types
            Assert.AreEqual(v1["aString"].Type, Variant.EnumType.String);
            Assert.AreEqual(v1["aBoolean"].Type, Variant.EnumType.Boolean);
            Assert.AreEqual(v1["aInteger"].Type, Variant.EnumType.Int64);
            Assert.AreEqual(v1["aUnsigned"].Type, Variant.EnumType.UInt64);
            Assert.AreEqual(v1["aInt32"].Type, Variant.EnumType.Int32);
            Assert.AreEqual(v1["aUInt32"].Type, Variant.EnumType.UInt32);
            Assert.AreEqual(v1["aInt64"].Type, Variant.EnumType.Int64);
            Assert.AreEqual(v1["aUInt64"].Type, Variant.EnumType.UInt64);
            Assert.AreEqual(v1["aFloat"].Type, Variant.EnumType.Double);
            Assert.AreEqual(v1["aDouble"].Type, Variant.EnumType.Double);
            Assert.AreEqual(v1["aDate"].Type, Variant.EnumType.DateTime);
            Assert.AreEqual(v1["aTime"].Type, Variant.EnumType.Time);
            Assert.AreEqual(v1["aDateTime"].Type, Variant.EnumType.DateTime);

            // Check node types
            Assert.AreEqual(v1["String"].Type, Variant.EnumType.String);
            Assert.AreEqual(v1["Boolean"].Type, Variant.EnumType.Boolean);
            Assert.AreEqual(v1["Integer"].Type, Variant.EnumType.Int64);
            Assert.AreEqual(v1["Unsigned"].Type, Variant.EnumType.UInt64);
            Assert.AreEqual(v1["Int32"].Type, Variant.EnumType.Int32);
            Assert.AreEqual(v1["UInt32"].Type, Variant.EnumType.UInt32);
            Assert.AreEqual(v1["Int64"].Type, Variant.EnumType.Int64);
            Assert.AreEqual(v1["UInt64"].Type, Variant.EnumType.UInt64);
            Assert.AreEqual(v1["Float"].Type, Variant.EnumType.Double);
            Assert.AreEqual(v1["Double"].Type, Variant.EnumType.Double);
            Assert.AreEqual(v1["Date"].Type, Variant.EnumType.DateTime);
            Assert.AreEqual(v1["Time"].Type, Variant.EnumType.Time);
            Assert.AreEqual(v1["DateTime"].Type, Variant.EnumType.DateTime);
        }
    }
}
