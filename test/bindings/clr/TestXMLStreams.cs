//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Text;

namespace protean
{
    namespace clr
    {
        namespace test
        {
            using NUnit.Framework;
            using protean.clr;

            [TestFixture]
            public class TestXMLStreams
            {
                [Test]
                public void TestDictionary()
                {
                    Variant input = new Variant(Variant.EnumType.Dictionary);
                    input.Add("key1", new Variant("value1"));
                    input.Add("key2", new Variant("value2"));

                    StringBuilder ibuffer = new StringBuilder();
                    System.IO.StringWriter istream = new System.IO.StringWriter(ibuffer);
                    XMLWriter writer = new XMLWriter(istream);
                    writer.Write(input);

                    System.IO.StringReader ostream = new System.IO.StringReader(ibuffer.ToString());
                    XMLReader reader = new XMLReader(ostream);
                    Variant output = reader.Read();

                    Assert.True(input.Equals(output));
                }

                [Test]
                public void TestSchemaValidation()
                {
                    System.IO.StringReader xml = new System.IO.StringReader(
                        @"<?xml version=""1.0"" encoding=""utf-8""?>
                          <Variant aBoolean=""true"" aDate=""2007-01-03"" aDateTime=""2007-01-03T10:30:00"" aFloat=""0.5"" aDouble=""3.9"" aInteger=""1"" aUnsigned=""2"" aInt32=""3"" aUInt32=""4"" aInt64=""5"" aUInt64=""6"" aString=""Element"" aTime=""10:30:00"">
                            <Boolean>true</Boolean>
                            <Date>2007-01-03</Date>
                            <DateTime>2007-01-03T10:30:00</DateTime>
                            <Float>0.5</Float>
                            <Double>1</Double>
                            <Integer>1</Integer>
                            <Unsigned>2</Unsigned>
                            <Int32>3</Int32>
                            <UInt32>4</UInt32>
                            <Int64>5</Int64>
                            <UInt64>6</UInt64>
                            <String>Element</String>
                            <Time>10:30:00</Time>
                          </Variant>");

                    System.IO.StringReader xsd = new System.IO.StringReader(
                        @"<?xml version=""1.0"" encoding=""utf-8""?>
                          <xsd:schema xmlns:xsd=""http://www.w3.org/2001/XMLSchema"">
                            <xsd:element name=""Variant"">
                              <xsd:complexType mixed=""true"">
                                <xsd:all>
                                  <xsd:element name=""String""   type=""xsd:string""/>
                                  <xsd:element name=""Boolean""  type=""xsd:boolean""/>
                                  <xsd:element name=""Integer""  type=""xsd:integer""/>
                                  <xsd:element name=""Unsigned"" type=""xsd:positiveInteger""/>
                                  <xsd:element name=""Int32""    type=""xsd:int""/>
                                  <xsd:element name=""UInt32""   type=""xsd:unsignedInt""/>
                                  <xsd:element name=""Int64""    type=""xsd:long""/>
                                  <xsd:element name=""UInt64""   type=""xsd:unsignedLong""/>
                                  <xsd:element name=""Float""    type=""xsd:float""/>
                                  <xsd:element name=""Double""   type=""xsd:double""/>
                                  <xsd:element name=""Date""     type=""xsd:date""/>
                                  <xsd:element name=""Time""     type=""xsd:time""/>
                                  <xsd:element name=""DateTime"" type=""xsd:dateTime""/>
                                </xsd:all>
                                <xsd:attribute name=""variant""   type=""xsd:string"" use=""optional"" default=""Dictionary""/>
                                <xsd:attribute name=""aString""   type=""xsd:string""/>
                                <xsd:attribute name=""aBoolean""  type=""xsd:boolean""/>
                                <xsd:attribute name=""aInteger""  type=""xsd:integer""/>
                                <xsd:attribute name=""aUnsigned"" type=""xsd:positiveInteger""/>
                                <xsd:attribute name=""aInt32""    type=""xsd:int""/>
                                <xsd:attribute name=""aUInt32""   type=""xsd:unsignedInt""/>
                                <xsd:attribute name=""aInt64""    type=""xsd:long""/>
                                <xsd:attribute name=""aUInt64""   type=""xsd:unsignedLong""/>
                                <xsd:attribute name=""aFloat""    type=""xsd:float""/>
                                <xsd:attribute name=""aDouble""   type=""xsd:double""/>
                                <xsd:attribute name=""aDate""     type=""xsd:date""/>
                                <xsd:attribute name=""aTime""     type=""xsd:time""/>
                                <xsd:attribute name=""aDateTime"" type=""xsd:dateTime""/>
                              </xsd:complexType>
                            </xsd:element>
                          </xsd:schema>");

                    Variant v1;
                    using (XMLReader reader = new XMLReader(xml))
                    {
                        reader.SetExternalSchema("MySchema");
                        reader.AddEntityReader("MySchema", xsd);

                        v1 = reader.Read();
                    }

                    Assert.AreEqual(v1["String"].Type, Variant.EnumType.String);
                    Assert.AreEqual(v1["Boolean"].Type, Variant.EnumType.Boolean);
                    Assert.AreEqual(v1["Integer"].Type, Variant.EnumType.Int32);
                    Assert.AreEqual(v1["Unsigned"].Type, Variant.EnumType.UInt32);
                    Assert.AreEqual(v1["Int32"].Type, Variant.EnumType.Int32);
                    Assert.AreEqual(v1["UInt32"].Type, Variant.EnumType.UInt32);
                    Assert.AreEqual(v1["Int64"].Type, Variant.EnumType.Int64);
                    Assert.AreEqual(v1["UInt64"].Type, Variant.EnumType.UInt64);
                    Assert.AreEqual(v1["Float"].Type, Variant.EnumType.Float);
                    Assert.AreEqual(v1["Double"].Type, Variant.EnumType.Double);
                    Assert.AreEqual(v1["Date"].Type, Variant.EnumType.Date);
                    Assert.AreEqual(v1["Time"].Type, Variant.EnumType.Time);
                    Assert.AreEqual(v1["DateTime"].Type, Variant.EnumType.DateTime);

                    // check attribute types
                    Assert.AreEqual(v1["aString"].Type, Variant.EnumType.String);
                    Assert.AreEqual(v1["aBoolean"].Type, Variant.EnumType.Boolean);
                    Assert.AreEqual(v1["aInteger"].Type, Variant.EnumType.Int32);
                    Assert.AreEqual(v1["aUnsigned"].Type, Variant.EnumType.UInt32);
                    Assert.AreEqual(v1["aInt32"].Type, Variant.EnumType.Int32);
                    Assert.AreEqual(v1["aUInt32"].Type, Variant.EnumType.UInt32);
                    Assert.AreEqual(v1["aInt64"].Type, Variant.EnumType.Int64);
                    Assert.AreEqual(v1["aUInt64"].Type, Variant.EnumType.UInt64);
                    Assert.AreEqual(v1["aFloat"].Type, Variant.EnumType.Float);
                    Assert.AreEqual(v1["aDouble"].Type, Variant.EnumType.Double);
                    Assert.AreEqual(v1["aDate"].Type, Variant.EnumType.Date);
                    Assert.AreEqual(v1["aTime"].Type, Variant.EnumType.Time);
                    Assert.AreEqual(v1["aDateTime"].Type, Variant.EnumType.DateTime);
                }
            }
        }
    }
}
