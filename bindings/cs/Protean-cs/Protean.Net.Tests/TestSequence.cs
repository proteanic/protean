using System;
using System.Collections.Generic;

namespace Protean.Test
{
    using NUnit.Framework;

    [TestFixture]
    public class TestSequence
    {
        [Test]
        public void TestListConstruction()
        {
            Variant v = new Variant(Variant.EnumType.List);
            Assert.AreEqual(v.Type, Variant.EnumType.List);
            Assert.IsTrue(v.Is(Variant.EnumType.List));
            Assert.AreEqual(v.Count, 0);
            Assert.IsTrue(v.Empty);  
        }

        [Test]
        public void TestListAdd()
        {
            Variant v = new Variant(Variant.EnumType.List);

            Assert.Throws<VariantException>(delegate { v.Add("key", new Variant()); });
            Assert.Throws<VariantException>(delegate { v.Add(new DateTime(0), new Variant()); });

            v.Add(new Variant("value1"));
            v.Add(new Variant(2.0));

            Assert.AreEqual(v.Count, 2);
            Assert.IsFalse(v.Empty);

            Assert.AreEqual(Variant.EnumType.String, v[0].Type);
            Assert.AreEqual("value1", v[0].As<string>());
            Assert.AreEqual(Variant.EnumType.Double, v[1].Type);
            Assert.AreEqual(2.0, v[1].As<double>());

            Assert.Throws<VariantException>(delegate { Variant w = v[-1]; });
            Assert.Throws<VariantException>(delegate { Variant w = v[2]; });

            v[0] = new Variant("other");
            Assert.AreEqual(v[0].As<string>(), "other");

            v.Clear();
            Assert.AreEqual(v.Count, 0);
            Assert.IsTrue(v.Empty);
        }

        [Test]
        public void TestCollectionInitialization()
        {
            Variant v = new Variant(Variant.EnumType.List) {
                new Variant("value1"),
                new Variant(2.0)
            };

            Assert.AreEqual(v.Count, 2);
            Assert.AreEqual(Variant.EnumType.String, v[0].Type);
            Assert.AreEqual("value1", v[0].As<string>());
            Assert.AreEqual(Variant.EnumType.Double, v[1].Type);
            Assert.AreEqual(2.0, v[1].As<double>());
        }

        [Test]
        public void TestListEnumeration()
        {
            Variant v = new Variant(Variant.EnumType.List);

            v.Add(new Variant("value1"));
            v.Add(new Variant(2.0));

            IEnumerator<VariantItem> en = v.GetEnumerator();

            en.Reset();
            Assert.IsTrue(en.MoveNext());
            Assert.Throws<VariantException>(delegate { string k = en.Current.Key; });
            Assert.Throws<VariantException>(delegate { DateTime t = en.Current.Time; });
            Assert.AreEqual("value1", en.Current.Value.As<string>());
            Assert.IsTrue(en.MoveNext());
            Assert.Throws<VariantException>(delegate { string k = en.Current.Key; });
            Assert.Throws<VariantException>(delegate { DateTime t = en.Current.Time; });
            Assert.AreEqual(2.0, en.Current.Value.As<double>());
            Assert.IsFalse(en.MoveNext());
        }

        [Test]
        public void TestListComparison()
        {
            Variant v1 = new Variant(Variant.EnumType.List);
            v1.Add(new Variant("value1"));
            v1.Add(new Variant("value2"));

            Variant v2 = new Variant(Variant.EnumType.List);
            v2.Add(new Variant("value2"));
            v2.Add(new Variant("value1"));

            Assert.AreEqual(0, v1.CompareTo(v1));
            Assert.AreEqual(-1, v1.CompareTo(v2));
            Assert.AreEqual(1, v2.CompareTo(v1));
        }

        [Test]
        public void TestTupleConstruction()
        {
            Variant v = new Variant(Variant.EnumType.Tuple, 2);
            Assert.AreEqual(v.Type, Variant.EnumType.Tuple);
            Assert.IsTrue(v.Is(Variant.EnumType.Tuple));
            Assert.AreEqual(v.Count, 2);
            Assert.IsFalse(v.Empty);
        }

        [Test]
        public void TestTupleAdd()
        {
            Variant v = new Variant(Variant.EnumType.Tuple, 2);

            Assert.Throws<VariantException>(delegate { v.Add(new Variant()); });
            Assert.Throws<VariantException>(delegate { v.Add("key", new Variant()); });
            Assert.Throws<VariantException>(delegate { v.Add(new DateTime(0), new Variant()); });

            v[0] = new Variant("value1");
            v[1] = new Variant(2.0);

            Assert.AreEqual(v.Count, 2);
            Assert.IsFalse(v.Empty);

            Assert.AreEqual(Variant.EnumType.String, v[0].Type);
            Assert.AreEqual("value1", v[0].As<string>());
            Assert.AreEqual(Variant.EnumType.Double, v[1].Type);
            Assert.AreEqual(2.0, v[1].As<double>());

            Assert.Throws<VariantException>(delegate { Variant w = v[-1]; });
            Assert.Throws<VariantException>(delegate { Variant w = v[2]; });

            v[0] = new Variant("other");
            Assert.AreEqual(v[0].As<string>(), "other");

            v.Clear();
            Assert.AreEqual(v[0].Type, Variant.EnumType.None);
            Assert.AreEqual(v[1].Type, Variant.EnumType.None);
        }

        [Test]
        public void TestTupleEnumeration()
        {
            Variant v = new Variant(Variant.EnumType.Tuple, 2);

            v[0] = new Variant("value1");
            v[1] = new Variant(2.0);

            IEnumerator<VariantItem> en = v.GetEnumerator();

            en.Reset();
            Assert.IsTrue(en.MoveNext());
            Assert.Throws<VariantException>(delegate { string k = en.Current.Key; });
            Assert.Throws<VariantException>(delegate { DateTime t = en.Current.Time; });
            Assert.AreEqual("value1", en.Current.Value.As<string>());
            Assert.IsTrue(en.MoveNext());
            Assert.Throws<VariantException>(delegate { string k = en.Current.Key; });
            Assert.Throws<VariantException>(delegate { DateTime t = en.Current.Time; });
            Assert.AreEqual(2.0, en.Current.Value.As<double>());
            Assert.IsFalse(en.MoveNext());
        }

        [Test]
        public void TestTupleComparison()
        {
            Variant v1 = new Variant(Variant.EnumType.Tuple, 2);
            v1[0] = new Variant("value1");
            v1[1] = new Variant("value2");

            Variant v2 = new Variant(Variant.EnumType.Tuple, 2);
            v2[0] = new Variant("value2");
            v2[1] = new Variant("value1");

            Assert.AreEqual(0, v1.CompareTo(v1));
            Assert.AreEqual(-1, v1.CompareTo(v2));
            Assert.AreEqual(1, v2.CompareTo(v1));
        }
    }
}
