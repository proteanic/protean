namespace Protean.Test
{
    using NUnit.Framework;

    [TestFixture]
    public class TestBuffer
    {
        [Test]
        public void TestBasic()
        {
            Variant v = new Variant(new byte[] { 0x01, 0x02, 0x03 });
            Assert.AreEqual(v.Type, Variant.EnumType.Buffer);
            Assert.IsTrue(v.Is(Variant.EnumType.Buffer));
            Assert.IsTrue(SequenceComparer.Compare(v.AsBuffer(), new byte[] { 0x01, 0x02, 0x03 })==0); 
        }

        [Test]
        public void TestComparison()
        {
            Variant v1 = new Variant(new byte[] { 0x01, 0x02, 0x03 });
            Variant v2 = new Variant(new byte[] { 0x01, 0x02, 0x04 });
            Variant v3 = new Variant(new byte[] { 0x01, 0x02, 0x03, 0x04 });

            Assert.AreEqual(0, v1.CompareTo(v1));
            Assert.AreEqual(-1, v1.CompareTo(v2));
            Assert.AreEqual(1, v2.CompareTo(v1));
            Assert.AreEqual(-1, v1.CompareTo(v3));
            Assert.AreEqual(1, v3.CompareTo(v1));
        }

        [Test]
        public void TestHashCode()
        {
            Variant v1 = new Variant(new byte[] { 0x01, 0x02, 0x03, 0x04 });
            Variant v2 = new Variant(new byte[] { 0x01, 0x02, 0x03, 0x04 });

            Assert.AreEqual(v1, v2);
            Assert.AreNotSame(v1, v2);
            Assert.AreEqual(v1.GetHashCode(), v2.GetHashCode());
        }
    }
}
