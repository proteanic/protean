#if !DISABLE_DATATABLE
//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

using System;

using System.Data;

using System.Runtime.Serialization.Formatters.Binary;
using System.IO;

namespace Protean.Test
{
    using NUnit.Framework;

    [TestFixture]
    public class TestDataTable
    {
        public void TestConstruction()
        {
            System.Data.DataTable dt1 = new DataTable();
            dt1.Columns.Add(new DataColumn("Double", typeof(double)));
            dt1.Columns.Add(new DataColumn("String", typeof(string)));

            DataRow dr = dt1.NewRow();
            dr[0] = 1.0;
            dr[1] = "value";

            dt1.Rows.Add(dr);

            Variant v1 = new Variant(dt1);
            Assert.AreEqual(v1.Type, Variant.EnumType.DataTable);
            Assert.IsTrue(v1.Is(Variant.EnumType.DataTable));

            Assert.DoesNotThrow(delegate { v1.AsDataTable(); });
            Assert.Throws<VariantException>(delegate { v1.As<int>(); });
        }

        [Test]
        public void TestComparison()
        {
            System.Data.DataTable dt1 = new DataTable();
            dt1.Columns.Add(new DataColumn("Double", typeof(double)));
            dt1.Columns.Add(new DataColumn("String", typeof(string)));

            DataRow dr1 = dt1.NewRow();
            dr1[0] = 1.0;
            dr1[1] = "value";

            dt1.Rows.Add(dr1);

            Variant v1 = new Variant(dt1);

            System.Data.DataTable dt2 = new DataTable();
            dt2.Columns.Add(new DataColumn("Double", typeof(double)));
            dt2.Columns.Add(new DataColumn("String", typeof(string)));

            DataRow dr2 = dt2.NewRow();
            dr2[0] = 1.0;
            dr2[1] = "value";

            dt2.Rows.Add(dr2);

            Variant v2 = new Variant(dt2);

            System.Data.DataTable dt3 = new DataTable();
            dt3.Columns.Add(new DataColumn("Double", typeof(double)));
            dt3.Columns.Add(new DataColumn("String", typeof(string)));

            DataRow dr3 = dt3.NewRow();
            dr3[0] = 2.0;
            dr3[1] = "value";

            dt3.Rows.Add(dr3);

            Variant v3 = new Variant(dt3);

            Assert.IsTrue(v1.Equals(v2));
            Assert.IsFalse(v1.Equals(v3));

            Assert.AreEqual(0, v1.CompareTo(v2));
            Assert.AreEqual(-1, v2.CompareTo(v3));
            Assert.AreEqual(1, v3.CompareTo(v2));
        }

        [Test]
        public void TestBinaryStreams()
        {
            System.Data.DataTable dt1 = new DataTable();
            dt1.Columns.Add(new DataColumn("Float", typeof(float)));
            dt1.Columns.Add(new DataColumn("Double", typeof(double)));
            dt1.Columns.Add(new DataColumn("String", typeof(string)));
            dt1.Columns.Add(new DataColumn("Boolean", typeof(bool)));
            dt1.Columns.Add(new DataColumn("Int32", typeof(int)));
            dt1.Columns.Add(new DataColumn("UInt32", typeof(uint)));
            dt1.Columns.Add(new DataColumn("Int64", typeof(long)));
            dt1.Columns.Add(new DataColumn("UInt64", typeof(ulong)));
            dt1.Columns.Add(new DataColumn("Time", typeof(TimeSpan)));
            dt1.Columns.Add(new DataColumn("DateTime", typeof(DateTime)));

            for (int i = 0; i < 10; ++i)
            {
                DataRow dr = dt1.NewRow();
                dr[0] = 1.0;
                dr[1] = 2.0;
                dr[2] = "value";
                dr[3] = true;
                dr[4] = 4;
                dr[5] = 5;
                dr[6] = 6;
                dr[7] = 7;
                dr[8] = new TimeSpan(1, 2, 3, 4, 5);
                dr[9] = new DateTime(2010, 1, 2, 3, 4, 5, 6);

                dt1.Rows.Add(dr);
            }

            Variant v1 = new Variant(dt1);

            byte[] bytes = BinaryWriter.ToBytes(v1);

            Variant v2 = BinaryReader.FromBytes(bytes);

            DataTable dt2 = v2.AsDataTable();

            Assert.AreEqual(dt1.Rows.Count, dt2.Rows.Count);
            Assert.AreEqual(dt1.Columns.Count, dt2.Columns.Count);

            for(int i=0; i<dt1.Columns.Count; ++i)
            {
                Assert.AreEqual(dt1.Columns[i].ColumnName, dt2.Columns[i].ColumnName);
                Assert.AreEqual(dt1.Columns[i].DataType, dt2.Columns[i].DataType);
            }

            Assert.AreEqual(1.0, (float)dt2.Rows[0][0]);
            Assert.AreEqual(2.0, (double)dt2.Rows[0][1]);
            Assert.AreEqual("value", (string)dt2.Rows[0][2]);
            Assert.AreEqual(true, (bool)dt2.Rows[0][3]);
            Assert.AreEqual(4, (int)dt2.Rows[0][4]);
            Assert.AreEqual(5, (uint)dt2.Rows[0][5]);
            Assert.AreEqual(6, (long)dt2.Rows[0][6]);
            Assert.AreEqual(7, (ulong)dt2.Rows[0][7]);
            Assert.AreEqual(new TimeSpan(1, 2, 3, 4, 5), (TimeSpan)dt2.Rows[0][8]);
            Assert.AreEqual(new DateTime(2010, 1, 2, 3, 4, 5, 6), (DateTime)dt2.Rows[0][9]);
        }

        [Test]
        public void TestPerformance()
        {
            System.Data.DataTable dt1 = new DataTable();
            dt1.Columns.Add(new DataColumn("column 0", typeof(double)));
            dt1.Columns.Add(new DataColumn("column 1", typeof(string)));
            dt1.Columns.Add(new DataColumn("column 2", typeof(DateTime)));

            dt1.RemotingFormat = SerializationFormat.Binary;

            for (int i = 0; i < 1000000; ++i)
            {
                DataRow dr1 = dt1.NewRow();
                dr1[0] = (double)i;
                dr1[1] = "value1";
                dr1[2] = new DateTime(2010, 1, 1);

                dt1.Rows.Add(dr1);
            }

            // Variant serialisation (uncompressed)
            DateTime t1 = DateTime.Now;

            byte[] bytes1 = BinaryWriter.ToBytes(new Variant(dt1));

            DateTime t2 = DateTime.Now;

            Variant v2 = BinaryReader.FromBytes(bytes1);

            DateTime t3 = DateTime.Now;

            System.Console.WriteLine("Variant serialisation size={0}, time={1}", bytes1.Length, t2 - t1);
            System.Console.WriteLine("Variant deserialisation, time={0}", t3 - t2);

            Assert.AreEqual(3, v2.AsDataTable().Columns.Count);
            Assert.AreEqual(1000000, v2.AsDataTable().Rows.Count);

            // Variant serialisation (compressed)
            DateTime t4 = DateTime.Now;

            byte[] bytes2 = BinaryWriter.ToBytes(new Variant(dt1), BinaryMode.Compress);

            DateTime t5 = DateTime.Now;

            Variant v3 = BinaryReader.FromBytes(bytes2);

            DateTime t6 = DateTime.Now;

            System.Console.WriteLine("Variant serialisation (compressed) size={0}, time={1}", bytes2.Length, t5 - t4);
            System.Console.WriteLine("Variant deserialisation (compressed), time={0}", t6 - t5);

            Assert.AreEqual(3, v3.AsDataTable().Columns.Count);
            Assert.AreEqual(1000000, v3.AsDataTable().Rows.Count);

            // .NET serialisation
            DateTime t7 = DateTime.Now;

            MemoryStream ms = new MemoryStream();
            BinaryFormatter bf = new BinaryFormatter();
            bf.Serialize(ms, dt1);

            DateTime t8 = DateTime.Now;

            ms.Position = 0; 
            DataTable dt2 = (DataTable)bf.Deserialize(ms);

            DateTime t9 = DateTime.Now;

            System.Console.WriteLine(".NET serialisation size={0}, time={1}", ms.GetBuffer().Length, t8 - t7);
            System.Console.WriteLine(".NET deserialisation time={0}", t9 - t8);

            Assert.AreEqual(3, dt2.Columns.Count);
            Assert.AreEqual(1000000, dt2.Rows.Count);
        }
    }
}
#endif
