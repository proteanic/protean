using System;
using System.Collections.Generic;

namespace Protean.Test
{
    using NUnit.Framework;

    [TestFixture]
    public class TestPrimitives
    {
        [Test]
        public void TestNone()
        {
            Variant v1 = new Variant();
            Assert.AreEqual(v1.Type, Variant.EnumType.None);
            Assert.IsTrue(v1.Is(Variant.EnumType.None));

            Variant v2 = new Variant(Variant.EnumType.None);
            Assert.AreEqual(v2.Type, Variant.EnumType.None);
            Assert.IsTrue(v2.Is(Variant.EnumType.None));

            Variant v3 = new Variant(v1);
            Assert.IsTrue(v1.Equals(v3));
        }

        [Test]
        public void TestAny()
        {
            Variant v1 = new Variant(Variant.EnumType.Any, "1.0");
            Assert.AreEqual(v1.Type, Variant.EnumType.Any);
            Assert.IsTrue(v1.Is(Variant.EnumType.Any));

            Assert.AreEqual(1.0, v1.As<double>());

            Variant v2 = new Variant(v1);
            Assert.IsTrue(v1.Equals(v2));

            // Enumerating an Any variant with content should throw an exception
            Assert.Throws<VariantException>(delegate { v1.GetEnumerator(); });

            // We should be able to enumerate an Any variant with no content as this is a common occurance when reading untyped XML
            Variant v3 = new Variant(Variant.EnumType.Any);
            IEnumerator<VariantItem> enumerator = v3.GetEnumerator();
            Assert.IsFalse(enumerator.MoveNext());
            Assert.Throws<VariantException>(delegate { VariantItem item = enumerator.Current; });
        }

        [Test]
        public void TestString()
        {
            Variant v1 = new Variant("test string");
            Assert.AreEqual(v1.Type, Variant.EnumType.String);
            Assert.IsTrue(v1.Is(Variant.EnumType.String));
            Assert.IsTrue(v1.Is<string>());
            Assert.AreEqual(v1.As<string>(), "test string");
            Assert.AreEqual(Convert.ToString(v1), "test string");

            Variant v2 = new Variant(v1);
            Assert.IsTrue(v1.Equals(v2));
        }

        [Test]
        public void TestBoolean()
        {
            Variant vTrue = new Variant(true);
            Assert.AreEqual(vTrue.Type, Variant.EnumType.Boolean);
            Assert.IsTrue(vTrue.Is(Variant.EnumType.Boolean));
            Assert.IsTrue(vTrue.Is<bool>());
            Assert.IsTrue(vTrue.As<bool>());
            Assert.IsTrue(Convert.ToBoolean(vTrue));
            Assert.AreEqual(vTrue.ToString(), "true");

            Variant vFalse = new Variant(false);
            Assert.AreEqual(vFalse.Type, Variant.EnumType.Boolean);
            Assert.IsTrue(vFalse.Is(Variant.EnumType.Boolean));
            Assert.IsTrue(vFalse.Is<bool>());
            Assert.IsFalse(vFalse.As<bool>());
            Assert.IsFalse(Convert.ToBoolean(vFalse));
            Assert.AreEqual(vFalse.ToString(), "false");

            Variant v1 = new Variant(vTrue);
            Assert.IsTrue(v1.Equals(vTrue));

            Variant v2 = new Variant(vFalse);
            Assert.IsTrue(v2.Equals(vFalse));
        }

        [Test]
        public void TestInt32()
        {
            Variant v1 = new Variant((Int32)1); 
            Assert.AreEqual(v1.Type, Variant.EnumType.Int32);
            Assert.IsTrue(v1.Is(Variant.EnumType.Int32));
            Assert.IsTrue(v1.Is<Int32>());
            Assert.AreEqual(v1.As<Int32>(), 1);
            Assert.AreEqual(Convert.ToInt32(v1), 1);
            Assert.AreEqual(v1.ToString(), "1");

            Variant v2 = new Variant(v1);
            Assert.IsTrue(v1.Equals(v2));
        }

        [Test]
        public void TestUInt32()
        {
            Variant v1 = new Variant((UInt32)2);
            Assert.AreEqual(v1.Type, Variant.EnumType.UInt32);
            Assert.IsTrue(v1.Is(Variant.EnumType.UInt32));
            Assert.IsTrue(v1.Is<UInt32>());
            Assert.AreEqual(v1.As<UInt32>(), 2);
            Assert.AreEqual(Convert.ToUInt32(v1), 2);
            Assert.AreEqual(v1.ToString(), "2");

            Variant v2 = new Variant(v1);
            Assert.IsTrue(v1.Equals(v2));
        }

        [Test]
        public void TestInt64()
        {
            Variant v1 = new Variant((Int64)3);
            Assert.AreEqual(v1.Type, Variant.EnumType.Int64);
            Assert.IsTrue(v1.Is(Variant.EnumType.Int64));
            Assert.IsTrue(v1.Is<Int64>());
            Assert.AreEqual(v1.As<Int64>(), 3);
            Assert.AreEqual(Convert.ToInt64(v1), 3);
            Assert.AreEqual(v1.ToString(), "3");

            Variant v2 = new Variant(v1);
            Assert.IsTrue(v1.Equals(v2));
        }

        [Test]
        public void TestUInt64()
        {
            Variant v1 = new Variant((UInt64)4);
            Assert.AreEqual(v1.Type, Variant.EnumType.UInt64);
            Assert.IsTrue(v1.Is(Variant.EnumType.UInt64));
            Assert.IsTrue(v1.Is<UInt64>());
            Assert.AreEqual(v1.As<UInt64>(), 4);
            Assert.AreEqual(Convert.ToUInt64(v1), 4);
            Assert.AreEqual(v1.ToString(), "4");

            Variant v2 = new Variant(v1);
            Assert.IsTrue(v1.Equals(v2));
        }

        [Test]
        public void TestDouble()
        {
            Variant v1 = new Variant((double)5.0);
            Assert.AreEqual(v1.Type, Variant.EnumType.Double);
            Assert.IsTrue(v1.Is(Variant.EnumType.Double));
            Assert.IsTrue(v1.Is<double>());
            Assert.AreEqual(v1.As<double>(), 5.0);
            Assert.AreEqual(Convert.ToDouble(v1), 5.0);
            Assert.AreEqual(v1.ToString(), "5");

            Variant vNaN = new Variant(double.NaN);
            Assert.AreEqual(vNaN.ToString(), "NaN");

            Variant vINF = new Variant(double.PositiveInfinity);
            Assert.AreEqual(vINF.ToString(), "INF");

            Variant vNEGINF = new Variant(double.NegativeInfinity);
            Assert.AreEqual(vNEGINF.ToString(), "-INF");

            Variant v2 = new Variant(v1);
            Assert.IsTrue(v1.Equals(v2));
        }

        [Test]
        public void TestDateTime()
        {
            DateTime arg = new DateTime(2010, 1, 2, 3, 4, 5, 6);
            Variant v1 = new Variant(arg);
            Assert.AreEqual(v1.Type, Variant.EnumType.DateTime);
            Assert.IsTrue(v1.Is(Variant.EnumType.DateTime));
            Assert.IsTrue(v1.Is<DateTime>());
            Assert.AreEqual(v1.As<DateTime>(), arg);
            Assert.AreEqual(Convert.ToDateTime(v1), arg);

            Assert.AreEqual(v1.ToString(), "2010-01-02T03:04:05.006");

            Variant v2 = new Variant(v1);
            Assert.IsTrue(v1.Equals(v2));
        }

        [Test]
        public void TestTime([Values("02:03:04.005", "-00:00:01.108", "01:03:23")] string asText)
        {
            TimeSpan arg = TimeSpan.Parse(asText);
            Variant v1 = new Variant(arg);
            Assert.AreEqual(v1.Type, Variant.EnumType.Time);
            Assert.IsTrue(v1.Is(Variant.EnumType.Time));
            Assert.IsTrue(v1.Is<TimeSpan>());
            Assert.AreEqual(v1.As<TimeSpan>(), arg);

            Assert.AreEqual(v1.ToString(), asText);

            Variant v2 = new Variant(v1);
            Assert.IsTrue(v1.Equals(v2));
        }

        [Test]
        public void TestParsing()
        {
            // Time
            Variant v1 = new Variant(Variant.EnumType.Any, "02:03:04.005");
            Assert.AreEqual(new TimeSpan(0, 2, 3, 4, 5), v1.As<TimeSpan>());

            Variant v2 = new Variant(Variant.EnumType.Any, "02:03:04");
            Assert.AreEqual(new TimeSpan(2, 3, 4), v2.As<TimeSpan>());

            // DateTime
            Variant v3 = new Variant(Variant.EnumType.Any, "2010-01-02T03:04:05.006");
            Assert.AreEqual(new DateTime(2010, 1, 2, 3, 4, 5, 6), v3.As<DateTime>());

            Variant v4 = new Variant(Variant.EnumType.Any, "2010-01-02T03:04:05");
            Assert.AreEqual(new DateTime(2010, 1, 2, 3, 4, 5, 0), v4.As<DateTime>());

            // Double
            Variant v5 = new Variant(Variant.EnumType.Any, "NaN");
            Assert.IsTrue(double.IsNaN(v5.As<Double>()));

            Variant v6 = new Variant(Variant.EnumType.Any, "INF");
            Assert.IsTrue(double.IsPositiveInfinity(v6.As<Double>()));

            Variant v7 = new Variant(Variant.EnumType.Any, "-INF");
            Assert.IsTrue(double.IsNegativeInfinity(v7.As<Double>()));

            // String
            Variant v8 = new Variant(Variant.EnumType.Any, "value");
            Assert.AreEqual("value", v8.As<string>());
        }
    }
}
