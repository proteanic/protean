//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;
using System.Collections.Generic;

namespace Protean.Test
{
    using NUnit.Framework;
    using Protean;

    [TestFixture]
    public class TestTimeSeries
    {
        [Test]
        public void TestTSConstruction()
        {
            Variant v = new Variant(Variant.EnumType.TimeSeries);
            Assert.AreEqual(v.Type, Variant.EnumType.TimeSeries);
            Assert.IsTrue(v.Is(Variant.EnumType.TimeSeries));
            Assert.AreEqual(v.Count, 0);
            Assert.IsTrue(v.Empty);
        }

        [Test]
        public void TestTSAdd()
        {
            Variant v = new Variant(Variant.EnumType.TimeSeries);

            Assert.Throws<VariantException>(delegate { v.Add(new Variant()); });
            Assert.Throws<VariantException>(delegate { v.Add("key", new Variant()); });

            v.Add(new DateTime(2010, 1, 2, 3, 4, 5, 6), new Variant("value1"));
            v.Add(new DateTime(2010, 1, 3, 3, 4, 5, 6), new Variant(1.0));

            Assert.AreEqual(v.Count, 2);
            Assert.IsFalse(v.Empty);
        }

        [Test]
        public void TestTSEnumeration()
        {
            Variant v = new Variant(Variant.EnumType.TimeSeries);

            v.Add(new DateTime(2010, 1, 2, 3, 4, 5, 6), new Variant("value1"));
            v.Add(new DateTime(2010, 1, 3, 3, 4, 5, 6), new Variant("value2"));

            IEnumerator<VariantItem> en = v.GetEnumerator();

            en.Reset();
            Assert.IsTrue(en.MoveNext());
            Assert.Throws<VariantException>(delegate { string k = en.Current.Key; });
            Assert.AreEqual("value1", en.Current.Value.As<string>());
            Assert.AreEqual(new DateTime(2010, 1, 2, 3, 4, 5, 6), en.Current.Time);
            Assert.IsTrue(en.MoveNext());
            Assert.Throws<VariantException>(delegate { string k = en.Current.Key; });
            Assert.AreEqual("value2", en.Current.Value.As<string>());
            Assert.AreEqual(new DateTime(2010, 1, 3, 3, 4, 5, 6), en.Current.Time);
            Assert.IsFalse(en.MoveNext());
        }

        [Test]
        public void TestTSComparison()
        {
            Variant v1 = new Variant(Variant.EnumType.TimeSeries);
            v1.Add(new DateTime(2010, 1, 2, 3, 4, 5, 6), new Variant("value1"));
            v1.Add(new DateTime(2010, 1, 3, 3, 4, 5, 6), new Variant("value2"));

            Variant v2 = new Variant(Variant.EnumType.TimeSeries);
            v2.Add(new DateTime(2010, 1, 3, 3, 4, 5, 6), new Variant("value1"));
            v2.Add(new DateTime(2010, 1, 2, 3, 4, 5, 6), new Variant("value2"));

            Variant v3 = new Variant(Variant.EnumType.TimeSeries);
            v3.Add(new DateTime(2010, 1, 2, 3, 4, 5, 6), new Variant("value2"));
            v3.Add(new DateTime(2010, 1, 3, 3, 4, 5, 6), new Variant("value1"));

            Assert.AreEqual(0, v1.CompareTo(v1));
            Assert.AreEqual(-1, v1.CompareTo(v2));
            Assert.AreEqual(1, v2.CompareTo(v1));
            Assert.AreEqual(-1, v1.CompareTo(v3));
            Assert.AreEqual(1, v3.CompareTo(v1));
        }
    }
}
