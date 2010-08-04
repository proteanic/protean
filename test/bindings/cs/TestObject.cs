using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace protean.test
{
    using NUnit.Framework;
    using protean;

    [TestFixture]
    public class TestObject
    {
        class TestObject1 : VariantObjectBase
        {
            public TestObject1(string value) {
                m_value = value;
            }

            public TestObject1() :
                this("default")
            { }

            public override string Class { get { return "TestObject1"; } }

            public override int Version { get { return 1; } }

            public override Variant Deflate()
            {
                Variant result = new Variant(Variant.EnumType.Dictionary);
                result.Add("value", new Variant(m_value));
                return result;
            }

            public override void Inflate(Variant param, int version)
            {
                m_value = param["value"].As<string>();
            }

            private string m_value;
        }

        class TestObject2 : VariantObjectBase
        {
            public TestObject2(string value) {
                m_value = value;
            }

            public TestObject2() :
                this("default")
            { }

            public override string Class { get { return "TestObject2"; } }

            public override int Version { get { return 1; } }

            public override Variant Deflate()
            {
                Variant result = new Variant(Variant.EnumType.Dictionary);
                result.Add("value", new Variant(m_value));
                return result;
            }

            public override void Inflate(Variant param, int version)
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
            obj3.Coerce(obj2);
            
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
    }
}
