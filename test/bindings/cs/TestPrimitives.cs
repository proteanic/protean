using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace protean.test
{
    using NUnit.Framework;
    using protean;

    [TestFixture]
    public class TestPrimitives
    {
        [Test]
        public void TestString()
        {
            Variant v = new Variant("test string");
            Assert.AreEqual(v.Type, Variant.EnumType.String);
            Assert.IsTrue(v.Is(Variant.EnumType.String));
            Assert.IsTrue(v.Is<string>());
            Assert.AreEqual(v.As<string>(), "test string");
            Assert.AreEqual(Convert.ToString(v), "test string");
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
        }

        [Test]
        public void TestInt32()
        {
            Variant v = new Variant((Int32)1); 
            Assert.AreEqual(v.Type, Variant.EnumType.Int32);
            Assert.IsTrue(v.Is(Variant.EnumType.Int32));
            Assert.IsTrue(v.Is<Int32>());
            Assert.AreEqual(v.As<Int32>(), 1);
            Assert.AreEqual(Convert.ToInt32(v), 1);
            Assert.AreEqual(v.ToString(), "1");
        }

        [Test]
        public void TestUInt32()
        {
            Variant v = new Variant((UInt32)2);
            Assert.AreEqual(v.Type, Variant.EnumType.UInt32);
            Assert.IsTrue(v.Is(Variant.EnumType.UInt32));
            Assert.IsTrue(v.Is<UInt32>());
            Assert.AreEqual(v.As<UInt32>(), 2);
            Assert.AreEqual(Convert.ToUInt32(v), 2);
            Assert.AreEqual(v.ToString(), "2");
        }

        [Test]
        public void TestInt64()
        {
            Variant v = new Variant((Int64)3);
            Assert.AreEqual(v.Type, Variant.EnumType.Int64);
            Assert.IsTrue(v.Is(Variant.EnumType.Int64));
            Assert.IsTrue(v.Is<Int64>());
            Assert.AreEqual(v.As<Int64>(), 3);
            Assert.AreEqual(Convert.ToInt64(v), 3);
            Assert.AreEqual(v.ToString(), "3");
        }

        [Test]
        public void TestUInt64()
        {
            Variant v = new Variant((UInt64)4);
            Assert.AreEqual(v.Type, Variant.EnumType.UInt64);
            Assert.IsTrue(v.Is(Variant.EnumType.UInt64));
            Assert.IsTrue(v.Is<UInt64>());
            Assert.AreEqual(v.As<UInt64>(), 4);
            Assert.AreEqual(Convert.ToUInt64(v), 4);
            Assert.AreEqual(v.ToString(), "4");
        }

        [Test]
        public void TestDouble()
        {
            Variant v = new Variant((double)5.0);
            Assert.AreEqual(v.Type, Variant.EnumType.Double);
            Assert.IsTrue(v.Is(Variant.EnumType.Double));
            Assert.IsTrue(v.Is<double>());
            Assert.AreEqual(v.As<double>(), 5.0);
            Assert.AreEqual(Convert.ToDouble(v), 5.0);
            Assert.AreEqual(v.ToString(), "5");

            Variant vNaN = new Variant(double.NaN);
            Assert.AreEqual(vNaN.ToString(), "NaN");

            Variant vINF = new Variant(double.PositiveInfinity);
            Assert.AreEqual(vINF.ToString(), "INF");

            Variant vNEGINF = new Variant(double.NegativeInfinity);
            Assert.AreEqual(vNEGINF.ToString(), "-INF");

            Assert.IsTrue(double.IsNaN(VariantBase.ParseDouble("NaN")));
            Assert.IsTrue(double.IsPositiveInfinity(VariantBase.ParseDouble("INF")));
            Assert.IsTrue(double.IsNegativeInfinity(VariantBase.ParseDouble("-INF")));
        }

        [Test]
        public void TestDateTime()
        {
            DateTime arg = new DateTime(2010, 1, 2, 3, 4, 5, 6);
            Variant v = new Variant(arg);
            Assert.AreEqual(v.Type, Variant.EnumType.DateTime);
            Assert.IsTrue(v.Is(Variant.EnumType.DateTime));
            Assert.IsTrue(v.Is<DateTime>());
            Assert.AreEqual(v.As<DateTime>(), arg);
            Assert.AreEqual(Convert.ToDateTime(v), arg);

            Assert.AreEqual(v.ToString(), "2010-01-02T03:04:05.006");

            Assert.AreEqual(VariantBase.ParseDateTime("2010-01-02T03:04:05.006"), new DateTime(2010, 1, 2, 3, 4, 5, 6));
            Assert.AreEqual(VariantBase.ParseDateTime("2010-01-02T03:04:05"), new DateTime(2010, 1, 2, 3, 4, 5, 0));
        }

        [Test]
        public void TestTime()
        {
            TimeSpan arg = new TimeSpan(0, 2, 3, 4, 5);
            Variant v = new Variant(arg);
            Assert.AreEqual(v.Type, Variant.EnumType.Time);
            Assert.IsTrue(v.Is(Variant.EnumType.Time));
            Assert.IsTrue(v.Is<TimeSpan>());
            Assert.AreEqual(v.As<TimeSpan>(), arg);

            Assert.AreEqual(v.ToString(), "02:03:04.005");

            Assert.AreEqual(VariantBase.ParseTime("02:03:04.005"), new TimeSpan(0, 2, 3, 4, 5));
            Assert.AreEqual(VariantBase.ParseTime("02:03:04"), new TimeSpan(2, 3, 4));
        }
    }
}
