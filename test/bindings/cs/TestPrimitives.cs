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

            Assert.IsTrue(double.IsNaN(VariantBase.ParseDouble("NaN")));
            Assert.IsTrue(double.IsPositiveInfinity(VariantBase.ParseDouble("INF")));
            Assert.IsTrue(double.IsNegativeInfinity(VariantBase.ParseDouble("-INF")));

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

            Assert.AreEqual(VariantBase.ParseDateTime("2010-01-02T03:04:05.006"), new DateTime(2010, 1, 2, 3, 4, 5, 6));
            Assert.AreEqual(VariantBase.ParseDateTime("2010-01-02T03:04:05"), new DateTime(2010, 1, 2, 3, 4, 5, 0));

            Variant v2 = new Variant(v1);
            Assert.IsTrue(v1.Equals(v2));
        }

        [Test]
        public void TestTime()
        {
            TimeSpan arg = new TimeSpan(0, 2, 3, 4, 5);
            Variant v1 = new Variant(arg);
            Assert.AreEqual(v1.Type, Variant.EnumType.Time);
            Assert.IsTrue(v1.Is(Variant.EnumType.Time));
            Assert.IsTrue(v1.Is<TimeSpan>());
            Assert.AreEqual(v1.As<TimeSpan>(), arg);

            Assert.AreEqual(v1.ToString(), "02:03:04.005");

            Assert.AreEqual(VariantBase.ParseTime("02:03:04.005"), new TimeSpan(0, 2, 3, 4, 5));
            Assert.AreEqual(VariantBase.ParseTime("02:03:04"), new TimeSpan(2, 3, 4));

            Variant v2 = new Variant(v1);
            Assert.IsTrue(v1.Equals(v2));
        }
    }
}
