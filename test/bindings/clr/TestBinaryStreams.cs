//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;
using System.Linq;
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
            public class TestBinaryStreams
            {
                [Test]
                public void TestStreams()
                {
                    Variant dict = new Variant(Variant.EnumType.Dictionary);
                    dict.Add("key1", new Variant("value1"));
                    dict.Add("key2", new Variant("value2"));

                    Variant input = new Variant(Variant.EnumType.List);
                    for (int i = 0; i < 10000; ++i)
                    {
                        input.Add(dict);
                    }

                    System.IO.MemoryStream istream = new System.IO.MemoryStream();
                    BinaryWriter writer = new BinaryWriter(istream);
                    writer.Write(input);

                    System.IO.MemoryStream ostream = new System.IO.MemoryStream(istream.GetBuffer());
                    BinaryReader reader = new BinaryReader(ostream);
                    Variant output = reader.Read();

                    Assert.True(input.Equals(output));
                }

                [Test]
                public void TestBytes()
                {
                    Variant dict = new Variant(Variant.EnumType.Dictionary);
                    dict.Add("key1", new Variant("value1"));
                    dict.Add("key2", new Variant("value2"));

                    Variant input = new Variant(Variant.EnumType.List);
                    for (int i = 0; i < 10000; ++i)
                    {
                        input.Add(dict);
                    }

                    byte[] bytes = BinaryWriter.ToBytes(input);
                    Variant output = BinaryReader.FromBytes(bytes);

                    Assert.True(input.Equals(output));
                }
            }
        }
    }
}
