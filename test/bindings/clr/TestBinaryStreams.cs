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
                public void TestDictionary()
                {
                    Variant input = new Variant(Variant.EnumType.Dictionary);
                    input.Add("key1", new Variant("value1"));
                    input.Add("key2", new Variant("value2"));

                    System.IO.MemoryStream istream = new System.IO.MemoryStream();
                    BinaryWriter writer = new BinaryWriter(istream);
                    writer.Write(input);

                    System.IO.MemoryStream ostream = new System.IO.MemoryStream(istream.GetBuffer());
                    BinaryReader reader = new BinaryReader(ostream);
                    Variant output = reader.Read();

                    Assert.True(input.Equals(output));
                }
            }
        }
    }
}
