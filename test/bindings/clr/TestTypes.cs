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
            public class TestTypes
            {
                [Test]
                public void TestString()
                {
                    String s = "hello";
                    Variant v = new Variant(s);

                    Assert.AreEqual(s, v.AsString());
                }

                [Test]
                [ExpectedException(typeof(VariantException))]
                public void TestStringAsDouble()
                {
                    String s = "hello";
                    Variant v = new Variant(s);

                    v.AsDouble();
                }

                [Test]
                public void TestDictionary()
                {
                    Variant v = new Variant(Variant.EnumType.Dictionary);

                    Assert.AreEqual(v.Count, 0);

                    Assert.IsFalse(v.ContainsKey("key1"));
                    Assert.IsFalse(v.ContainsKey("key2"));

                    v.Add("key1", new Variant("value1"));
                    v.Add("key2", new Variant("value2"));

                    Assert.AreEqual(v.Count, 2);

                    Assert.IsTrue(v.ContainsKey("key1"));
                    Assert.IsTrue(v.ContainsKey("key2"));

                    Assert.AreEqual(v["key1"].AsString(), "value1");
                    Assert.AreEqual(v["key2"].AsString(), "value2");
                }

                [Test]
                [ExpectedException(typeof(VariantException))]
                public void TestDictionaryMissingKey()
                {
                    Variant v = new Variant(Variant.EnumType.Dictionary);
                    v.Add("key1", new Variant("value1"));
                    v.Add("key2", new Variant("value2"));

                    Variant MissingValue = v["missing key"];
                }
            }
        }
    }
}
