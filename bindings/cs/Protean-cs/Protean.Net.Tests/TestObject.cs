namespace Protean.Test
{
    using NUnit.Framework;

    [TestFixture]
    public class TestObject
    {
        class TestObject1 : IVariantObject
        {
            public TestObject1(string value) {
                m_value = value;
            }

            public TestObject1() :
                this("default")
            { }

            public string Class { get { return "TestObject1"; } }

            public int Version { get { return 1; } }

            public Variant Deflate()
            {
                Variant result = new Variant(Variant.EnumType.Dictionary);
                result.Add("value", new Variant(m_value));
                return result;
            }

            public void Inflate(Variant param, int version)
            {
                m_value = param["value"].As<string>();
            }

            private string m_value;
        }

        class TestObject2 : IVariantObject
        {
            public TestObject2(string value) {
                m_value = value;
            }

            public TestObject2() :
                this("default")
            { }

            public string Class { get { return "TestObject2"; } }

            public int Version { get { return 1; } }

            public Variant Deflate()
            {
                Variant result = new Variant(Variant.EnumType.Dictionary);
                result.Add("value", new Variant(m_value));
                return result;
            }

            public void Inflate(Variant param, int version)
            {
                m_value = param["value"].As<string>();
            }

            public string m_value;
        }

        [Test]
        public void TestConstruction()
        {
            TestObject1 obj1 = new TestObject1("some value");

            Variant v1 = new Variant(obj1);
            Assert.AreEqual(v1.Type, Variant.EnumType.Object);

            TestObject1 obj2 = v1.AsObject<TestObject1>();
            Assert.AreEqual(obj1, obj2);

            Assert.Throws<VariantException>(delegate { v1.AsObject<TestObject2>(); });
            Assert.Throws<VariantException>(delegate { v1.AsObject<VariantObjectProxy>(); });

            VariantObjectProxy obj3 = new VariantObjectProxy(obj1.Class);
            obj3.Inflate(obj2.Deflate(), obj2.Version);
            
            Assert.AreEqual(obj3.Class, "TestObject1");
            Assert.AreEqual(obj3.Version, 1);
            Assert.AreEqual(obj3.Deflate().Type, Variant.EnumType.Dictionary);

            Variant v2 = new Variant(obj3);

            Assert.IsInstanceOf<VariantObjectProxy>(v2.AsObject());

            Assert.Throws<VariantException>(delegate { v1.AsObject<TestObject2>(); });

            TestObject1 obj5 = v2.AsObject<TestObject1>();
            Assert.AreEqual(obj5.Class, "TestObject1");
            Assert.AreEqual(obj5.Version, 1);

            Assert.IsInstanceOf<TestObject1>(v2.AsObject());
        }

        [Test]
        public void TestComparison()
        {
            Variant v1 = new Variant(new TestObject1("some value"));
            Variant v2 = new Variant(new TestObject1("some other value"));
            Variant v3 = new Variant(new TestObject2("some value"));

            Assert.AreEqual(0, v1.CompareTo(v1));
            Assert.AreEqual(1, v1.CompareTo(v2));
            Assert.AreEqual(-1, v2.CompareTo(v1));
            Assert.AreEqual(-1, v1.CompareTo(v3));
            Assert.AreEqual(1, v3.CompareTo(v1));
        }

        [Test]
        public void TestHashCode()
        {
            Variant v1 = new Variant(new TestObject1("some value"));
            Variant v2 = new Variant(new TestObject1("some value"));

            Assert.AreEqual(v1, v2);
            Assert.AreNotSame(v1, v2);
            Assert.AreEqual(v1.GetHashCode(), v2.GetHashCode());
        }
    }
}
