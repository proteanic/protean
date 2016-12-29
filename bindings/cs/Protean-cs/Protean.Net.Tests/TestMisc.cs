//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

namespace Protean.Test
{
    using NUnit.Framework;

    [TestFixture]
    public class TestMisc
    {
        [Test]
        public void TestSelect()
        {
            Variant v = new Variant(Variant.EnumType.Dictionary);
            v.Add("key1", new Variant(Variant.EnumType.Dictionary));
            v.Add("key2", new Variant(Variant.EnumType.Bag));
            v["key1"].Add("key3", new Variant("value1"));
            v["key1"].Add("key4", new Variant("value2"));
            v["key2"].Add("key5", new Variant("value3"));
            v["key2"].Add("key6", new Variant("value4"));
            v["key2"].Add("key5", new Variant("value5"));

            Variant s1 = v.Select("/none/");
            Variant s2 = v.Select("/key1/");
            Variant s3 = v.Select("/key1/*");
            Variant s4 = v.Select("/key1/key3");
            Variant s5 = v.Select("/key1/key4");
            Variant s6 = v.Select("/key2/");
            Variant s7 = v.Select("/key2/*");
            Variant s8 = v.Select("/key2/key5");
            Variant s9 = v.Select("/key2/key6");

            Assert.AreEqual(0, s1.Count);
            Assert.AreEqual(1, s2.Count);
            Assert.AreEqual(2, s3.Count);
            Assert.AreEqual(1, s4.Count);
            Assert.AreEqual(1, s5.Count);
            Assert.AreEqual(1, s6.Count);
            Assert.AreEqual(3, s7.Count);
            Assert.AreEqual(2, s8.Count);
            Assert.AreEqual(1, s9.Count);
        }

        [Test]
        public void TestAnyCast()
        {
            double arg = 2.0;
            Variant v1 = new Variant(arg);
            Variant v2 = v1.AnyCast();

            Assert.AreEqual(Variant.EnumType.Any, v2.Type);
            Assert.AreEqual("2", v2.As<string>());
        }
    }
}
