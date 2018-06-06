using System;
using System.Collections.Generic;

namespace Protean.Test
{
    using NUnit.Framework;

    [TestFixture]
    public class TestMapping
    {
        [Test]
        public void TestDictionaryConstruction()
        {
            Variant v = new Variant(Variant.EnumType.Dictionary);
            Assert.AreEqual(v.Type, Variant.EnumType.Dictionary);
            Assert.IsTrue(v.Is(Variant.EnumType.Dictionary));
            Assert.AreEqual(v.Count, 0);
            Assert.IsTrue(v.Empty);  
        }

        [Test]
        public void TestDictionaryAdd()
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
        public void TestDictionaryEnumeration()
        {
            Variant v = new Variant(Variant.EnumType.Dictionary);

            v.Add("key1", new Variant("value1"));
            v.Add("key2", new Variant("value2"));

            int count = 1;
            foreach (VariantItem item in v)
            {
                Assert.AreEqual("key" + count, item.Key);
                Assert.AreEqual("value" + count, item.Value.As<string>());
                Assert.Throws<VariantException>(delegate { DateTime t = item.Time; });
                ++count;
            }
        }

        [Test]
        public void TestDictionaryComparison()
        {
            Variant v1 = new Variant(Variant.EnumType.Dictionary);
            v1.Add("key1", new Variant("value1"));
            v1.Add("key2", new Variant("value2"));

            Variant v2 = new Variant(Variant.EnumType.Dictionary);
            v2.Add("key2", new Variant("value2"));
            v2.Add("key1", new Variant("value1"));

            Variant v3 = new Variant(Variant.EnumType.Dictionary);
            v3.Add("key2", new Variant("value1"));
            v3.Add("key3", new Variant("value2"));

            Variant v4 = new Variant(Variant.EnumType.Dictionary);
            v4.Add("key1", new Variant("value2"));
            v4.Add("key2", new Variant("value3"));

            Assert.AreEqual(0, v1.CompareTo(v2));
            Assert.AreEqual(-1, v1.CompareTo(v3));
            Assert.AreEqual(1, v3.CompareTo(v1));
            Assert.AreEqual(-1, v1.CompareTo(v4));
            Assert.AreEqual(1, v4.CompareTo(v1));
        }

        [Test]
        public void TestBagConstruction()
        {
            Variant v = new Variant(Variant.EnumType.Bag);
            Assert.AreEqual(v.Type, Variant.EnumType.Bag);
            Assert.IsTrue(v.Is(Variant.EnumType.Bag));
            Assert.AreEqual(v.Count, 0);
            Assert.IsTrue(v.Empty);
        }

        [Test]
        public void TestBagAdd()
        {
            Variant v = new Variant(Variant.EnumType.Bag);

            Assert.Throws<VariantException>(delegate { v.Add(new Variant()); });
            Assert.Throws<VariantException>(delegate { v.Add(new DateTime(0), new Variant()); });

            v.Add("key1", new Variant("value1"));
            v.Add("key2", new Variant(1.0));
            v.Add("key1", new Variant("value2"));

            Assert.AreEqual(v.Count, 3);
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
            Assert.AreEqual(1, v.Count);

            v.Remove("key2");
            Assert.AreEqual(v.Count, 0);
            Assert.IsTrue(v.Empty);
        }

        [Test]
        public void TestBagRange()
        {
            Variant v = new Variant(Variant.EnumType.Bag);

            v.Add("key1", new Variant("value1"));
            v.Add("key2", new Variant(1.0));
            v.Add("key1", new Variant("value2"));

            Variant r1 = v.Range("missing");
            Assert.AreEqual(Variant.EnumType.List, r1.Type);
            Assert.AreEqual(0, r1.Count);

            Variant r2 = v.Range("key1");
            Assert.AreEqual(Variant.EnumType.List, r2.Type);
            Assert.AreEqual(2, r2.Count);
            Assert.AreEqual("value1", r2[0].As<string>());
            Assert.AreEqual("value2", r2[1].As<string>());

            Variant r3 = v.Range("key2");
            Assert.AreEqual(Variant.EnumType.List, r2.Type);
            Assert.AreEqual(1, r3.Count);
            Assert.AreEqual(1.0, r3[0].As<double>());
        }

        [Test]
        public void TestBagEnumeration()
        {
            Variant v = new Variant(Variant.EnumType.Bag);

            v.Add("key2", new Variant("value2"));
            v.Add("key1", new Variant("value1"));
            v.Add("key1", new Variant("value1"));
            v.Add("key2", new Variant("value2"));

            IEnumerator<VariantItem> en = v.GetEnumerator();

            en.Reset();
            Assert.IsTrue(en.MoveNext());
            Assert.AreEqual("key2", en.Current.Key);
            Assert.AreEqual("value2", en.Current.Value.As<string>());
            Assert.IsTrue(en.MoveNext());
            Assert.AreEqual("key1", en.Current.Key);
            Assert.AreEqual("value1", en.Current.Value.As<string>());
            Assert.IsTrue(en.MoveNext());
            Assert.AreEqual("key1", en.Current.Key);
            Assert.AreEqual("value1", en.Current.Value.As<string>());
            Assert.IsTrue(en.MoveNext());
            Assert.AreEqual("key2", en.Current.Key);
            Assert.AreEqual("value2", en.Current.Value.As<string>());
            Assert.IsFalse(en.MoveNext());
        }

        [Test]
        public void TestBagComparison()
        {
            Variant v1 = new Variant(Variant.EnumType.Bag);
            v1.Add("key1", new Variant("value1"));
            v1.Add("key2", new Variant("value2"));

            Variant v2 = new Variant(Variant.EnumType.Bag);
            v2.Add("key1", new Variant("value1"));
            v2.Add("key2", new Variant("value2"));

            Variant v3 = new Variant(Variant.EnumType.Bag);
            v3.Add("key2", new Variant("value1"));
            v3.Add("key3", new Variant("value2"));

            Variant v4 = new Variant(Variant.EnumType.Bag);
            v4.Add("key1", new Variant("value2"));
            v4.Add("key2", new Variant("value3"));

            Assert.AreEqual(0, v1.CompareTo(v2));
            Assert.AreEqual(-1, v1.CompareTo(v3));
            Assert.AreEqual(1, v3.CompareTo(v1));
            Assert.AreEqual(-1, v1.CompareTo(v4));
            Assert.AreEqual(1, v4.CompareTo(v1));
        }
    }
}
