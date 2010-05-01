//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

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
            public class TestEnumerator
            {
                [Test]
                public void TestListEnumerator()
                {
                    Variant list = new Variant(Variant.EnumType.List);

                    for (int i = 0; i < 1; ++i)
                    {
                        list.Add(new Variant(i));
                    }

                    // Generic enumerator
                    int count = 0;
                    foreach (VariantItem item in list)
                    {
                        Assert.True(item.Value.AsInt32().Equals(count));
                        Assert.Throws(typeof(VariantException),
                            delegate { String key = item.Key; }
                        );
                        Assert.Throws(typeof(VariantException),
                            delegate { DateTime time = item.Time; }
                        );
                        ++count;
                    }

                    // Non-generic enumerator
                    count = 0;
                    foreach (var item in list)
                    {
                        Assert.True(item.Value.AsInt32().Equals(count));
                        Assert.Throws(typeof(VariantException),
                            delegate { String key = item.Key; }
                        );
                        Assert.Throws(typeof(VariantException),
                            delegate { DateTime time = item.Time; }
                        );
                        ++count;
                    }
                }

                [Test]
                public void TestDictionaryEnumerator()
                {
                    Variant dict = new Variant(Variant.EnumType.Dictionary);

                    for (int i = 0; i < 1; ++i)
                    {
                        dict.Add(i.ToString(), new Variant(i));
                    }

                    // Generic enumerator
                    int count = 0;
                    foreach (VariantItem item in dict)
                    {
                        Assert.True(item.Value.AsInt32().Equals(count));
                        Assert.True(item.Key.Equals(count.ToString()));
                        Assert.Throws(typeof(VariantException),
                            delegate { DateTime time = item.Time; }
                        );
                        ++count;
                    }

                    // Non-generic enumerator
                    count = 0;
                    foreach (var item in dict)
                    {
                        Assert.True(item.Value.AsInt32().Equals(count));
                        Assert.True(item.Key.Equals(count.ToString()));
                        Assert.Throws(typeof(VariantException),
                            delegate { DateTime time = item.Time; }
                        );
                        ++count;
                    }
                }
            }
        }
    }
}
