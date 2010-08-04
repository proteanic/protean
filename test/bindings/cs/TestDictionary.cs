using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace protean.test
{
    using NUnit.Framework;
    using protean;

    [TestFixture]
    public class TestDictionary
    {
        [Test]
        public void TestEmpty()
        {
            Variant v = new Variant(Variant.EnumType.Dictionary);
            Assert.AreEqual(v.Type, Variant.EnumType.Dictionary);
            Assert.IsTrue(v.Is(Variant.EnumType.Dictionary));
            Assert.AreEqual(v.Count, 0);
            Assert.IsTrue(v.Empty);  
        }

        [Test]
        public void TestAdd()
        {
            Variant v = new Variant(Variant.EnumType.Dictionary);

            Assert.Throws<VariantException>(delegate { v.Add(new Variant()); });
            Assert.Throws<VariantException>(delegate { v.Add(new DateTime(0), new Variant()); });

            v.Add("key1", new Variant("value1"));
            v.Add("key2", new Variant(1.0));

            Assert.AreEqual(v.Count, 2);
            Assert.IsFalse(v.Empty);

            Assert.IsTrue(v.ContainsKey("key1"));
            Assert.IsTrue(v.ContainsKey("key2"));

            Assert.AreEqual(v["key1"].Type, Variant.EnumType.String);
            Assert.AreEqual(v["key1"].As<string>(), "value1");
            Assert.AreEqual(v["key2"].Type, Variant.EnumType.Double);
            Assert.AreEqual(v["key2"].As<double>(), 1.0);

            v["key1"] = new Variant("other");
            Assert.AreEqual(v["key1"].As<string>(), "other");

            v.Remove("key1");
            Assert.AreEqual(v.Count, 1);

            v.Remove("key2");
            Assert.AreEqual(v.Count, 0);
            Assert.IsTrue(v.Empty);
        }

        [Test]
        public void TestEnumeration()
        {
            Variant v = new Variant(Variant.EnumType.Dictionary);

            Assert.Throws<VariantException>(delegate { v.Add(new Variant()); });
            Assert.Throws<VariantException>(delegate { v.Add(new DateTime(0), new Variant()); });

            v.Add("key1", new Variant("value1"));
            v.Add("key2", new Variant("value2"));

            int count = 1;
            foreach (VariantItem item in v)
            {
                Assert.AreEqual(item.Key, "key" + count);
                Assert.AreEqual(item.Value.As<string>(), "value" + count);
                Assert.Throws<VariantException>(delegate { DateTime t = item.Time; });
                ++count;
            }
        }
    }
}
