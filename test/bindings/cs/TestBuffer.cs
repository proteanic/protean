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
    public class TestBuffer
    {
        [Test]
        public void TestBasic()
        {
            Variant v = new Variant(new byte[] { 0x01, 0x02, 0x03 });
            Assert.AreEqual(v.Type, Variant.EnumType.Buffer);
            Assert.IsTrue(v.Is(Variant.EnumType.Buffer));
            Assert.IsTrue(SequenceComparer.Compare(v.AsBuffer(), new byte[] { 0x01, 0x02, 0x03 })==0); 
        }

        [Test]
        public void TestComparison()
        {
            Variant v1 = new Variant(new byte[] { 0x01, 0x02, 0x03 });
            Variant v2 = new Variant(new byte[] { 0x01, 0x02, 0x04 });
            Variant v3 = new Variant(new byte[] { 0x01, 0x02, 0x03, 0x04 });

            Assert.AreEqual(0, v1.CompareTo(v1));
            Assert.AreEqual(-1, v1.CompareTo(v2));
            Assert.AreEqual(1, v2.CompareTo(v1));
            Assert.AreEqual(-1, v1.CompareTo(v3));
            Assert.AreEqual(1, v3.CompareTo(v1));
        }
    }
}
