//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

namespace Protean.Test
{
    using NUnit.Framework;

    [TestFixture]
    public class TestArray
    {
        public void TestConstruction()
        {
            TypedArray a1 = new TypedArray(Variant.EnumType.Double, 2);
            a1[0] = 1.0;
            a1[1] = 2.0;

            Variant v1 = new Variant(a1);

            Assert.DoesNotThrow(delegate { v1.AsArray(); });
            Assert.Throws<VariantException>(delegate { v1.As<int>(); });
        }

        [Test]
        public void TestComparison()
        {
            TypedArray a1 = new TypedArray(Variant.EnumType.Double, 2);
            a1[0] = 1.0;
            a1[1] = 2.0;

            Variant v1 = new Variant(a1);

            TypedArray a2 = new TypedArray(Variant.EnumType.Double, 2);
            a2[0] = 1.0;
            a2[1] = 2.0;

            Variant v2 = new Variant(a2);

            TypedArray a3 = new TypedArray(Variant.EnumType.Double, 2);
            a3[0] = 2.0;
            a3[1] = 1.0;

            Variant v3 = new Variant(a3);

 
            Assert.IsTrue(v1.Equals(v2));
            Assert.IsFalse(v1.Equals(v3));

            Assert.AreEqual(0, v1.CompareTo(v2));
            Assert.AreEqual(-1, v2.CompareTo(v3));
            Assert.AreEqual(1, v3.CompareTo(v2));
        }

        [Test]
        public void TestBinaryStreams()
        {
            TypedArray a1 = new TypedArray(Variant.EnumType.Double, 2);
            a1[0] = 1.0;
            a1[1] = 2.0;

            Variant v1 = new Variant(a1);

            byte[] bytes = BinaryWriter.ToBytes(v1);

            Variant v2 = BinaryReader.FromBytes(bytes);

            TypedArray a2 = v2.AsArray();

            Assert.AreEqual(a1.Count, a2.Count);
            Assert.AreEqual(a1.ElementType, a2.ElementType);

            Assert.AreEqual((double)a1[0], (double)a2[0]);
            Assert.AreEqual((double)a1[1], (double)a2[1]);
        }

        [Test]
        public void TestXMLStreams()
        {
            TypedArray a1 = new TypedArray(Variant.EnumType.Double, 2);
            a1[0] = 1.0;
            a1[1] = 2.0;

            Variant v1 = new Variant(a1);

            string xml = XmlWriter.ToString(v1);

            System.Console.WriteLine(xml);

            /*Variant v2 = BinaryReader.FromBytes(bytes);

            TypedArray a2 = v2.AsArray();

            Assert.AreEqual(a1.Count, a2.Count);
            Assert.AreEqual(a1.ElementType, a2.ElementType);

            Assert.AreEqual((double)a1[0], (double)a2[0]);
            Assert.AreEqual((double)a1[1], (double)a2[1]);*/
        }
    }
}
