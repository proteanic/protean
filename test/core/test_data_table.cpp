//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>  

using boost::unit_test::test_suite;    

#include <protean/variant.hpp>
#include <protean/detail/data_table.hpp>
#include <protean/xml_writer.hpp>
#include <protean/xml_reader.hpp>
#include <boost/date_time.hpp>
#include <protean/binary_writer.hpp>
#include <protean/binary_reader.hpp>
#include <iostream>
#include <boost/chrono/chrono.hpp>
#include <boost/chrono/chrono_io.hpp>
using namespace protean;

BOOST_AUTO_TEST_SUITE(data_table_suite);

BOOST_AUTO_TEST_CASE(test_data_table_basic)
{
    variant v1(variant::DataTable);
    BOOST_CHECK(v1.is<variant::DataTable>());
    BOOST_CHECK(v1.is<variant::Collection>());
    BOOST_CHECK(!v1.is<variant::Primitive>());
    BOOST_CHECK(!v1.is<variant::Mapping>());

    BOOST_CHECK(v1.empty());
    BOOST_CHECK_EQUAL(v1.size(), 0u);

    v1.add_column(variant::DateTime)
      .add_column(variant::Int32)
      .add_column(variant::Double);

    data_table_row<variant::DateTime, variant::Int32, variant::Double>::type
        row1 = boost::tuples::make_tuple(boost::posix_time::time_from_string("2002-01-20 09:00:00.000"), 42, 3.141);
    data_table_row<variant::DateTime, variant::Int32, variant::Double>::type
        row2 = boost::tuples::make_tuple(boost::posix_time::time_from_string("2002-01-20 10:00:00.000"), 1, 2.718);

    v1.push_back(row1)
      .push_back(row2);

    BOOST_CHECK(!v1.empty());
    BOOST_CHECK(v1.size()==2);

    BOOST_CHECK_THROW(v1.add_column(variant::String), variant_error);

    variant v2(variant::DataTable);
    v2.add_column(variant::DateTime)
      .add_column(variant::Int32)
      .add_column(variant::Double);
    BOOST_CHECK(v1!=v2);

    v2.push_back(row1)
      .push_back(row2);
    BOOST_CHECK(v1==v2);

    variant v3(v2);
    BOOST_CHECK(v3.is<variant::DataTable>());
    BOOST_CHECK(v3==v2);

    variant v4;
    v4 = v3;
    BOOST_CHECK(v4.is<variant::DataTable>());
    BOOST_CHECK(v4==v3);
}

BOOST_AUTO_TEST_CASE(test_data_table_types)
{
    variant v1(variant::DataTable);

    BOOST_CHECK_THROW(v1.add_column(variant::DataTable), variant_error);

    v1.add_column(variant::DateTime)
      .add_column(variant::Int32)
      .add_column(variant::Double);

    BOOST_CHECK_THROW(v1.push_back( boost::tuples::make_tuple(std::string("Incompatible columns")) ), variant_error);

    data_table_row<variant::DateTime, variant::Int32, variant::Double>::type
        row1 = boost::tuples::make_tuple(boost::posix_time::time_from_string("2002-01-20 09:00:00.000"), 42, 3.141);
    data_table_row<variant::DateTime, variant::Int32, variant::Double>::type
        row2 = boost::tuples::make_tuple(boost::posix_time::time_from_string("2002-01-20 10:00:00.000"), 1, 2.718);

    v1.push_back(row1)
      .push_back(row2);

    BOOST_CHECK_THROW(v1.begin<variant::String>(), variant_error);
    BOOST_CHECK_THROW(v1.end<variant::String>(), variant_error);
}

BOOST_AUTO_TEST_CASE(test_data_table_variant_fallback_types)
{
    variant dt(variant::DataTable);

    dt.add_column(variant::Int32)
      .add_column(variant::Double)
      .add_column(variant::Tuple);

    int x1 = 42;
    double x2 = 3.14;
    variant x3(variant::Tuple, 3);
    x3.at(0) = variant(100);
    x3.at(1) = variant(std::string("Hello"));
    x3.at(2) = variant(true);

    dt.push_back( make_row(x1, x2, x3) );

    variant wrong_variant(variant::Dictionary, 2);
    wrong_variant.insert("foo", variant(variant::date_t(2007, 1, 3)));
    wrong_variant.insert("bar", variant(variant::time_t(10, 30, 0)));

    // Wrong variant type for variant column
    BOOST_CHECK_THROW(dt.push_back( make_row(x1, x2, wrong_variant) ), variant_error);

    // Variant for typed column
    BOOST_CHECK_THROW(dt.push_back( make_row(wrong_variant, x2, x3) ), variant_error);


    /* Check push_back and typed iterators adhere to the variant enum inheritance hierarchy */
    variant dt2(variant::DataTable);
    dt2.add_column(variant::Int32)
       .add_column(variant::Sequence);

    variant xs(variant::List, 2);
    xs.at(0) = variant(0);
    xs.at(1) = variant(1);
    variant t(variant::Tuple);
    variant dict(variant::Dictionary);

    dt2.push_back( make_row(1, xs) ); // OK since xs is a variant list which is a variant sequence
    dt2.push_back( make_row(2, t) );  // OK since t is a variant tuple which is a variant sequence
    BOOST_CHECK_THROW(dt2.push_back( make_row(3, dict) ), variant_error);

    dt2.begin<variant::Int32, variant::Sequence>();
    dt2.begin<variant::Int32, variant::List>();
    BOOST_CHECK_THROW(( dt2.begin<variant::Int32, variant::Dictionary>() ), variant_error);
}

BOOST_AUTO_TEST_CASE(test_data_table_variant_fallback_iterators)
{
    variant v1(variant::DataTable);
    v1.add_column(variant::Int32)
      .add_column(variant::Tuple);

    variant t1(variant::Tuple, 2);
    t1.at(0) = variant(std::string("foo"));
    t1.at(1) = variant(2.718);
    v1.push_back( make_row(1, t1) );

    variant t2(variant::Tuple, 1);
    t2.at(0) = variant(42);
    v1.push_back( make_row(2, t2) );

    // Runtime error if we specify the wrong column type
    BOOST_CHECK_THROW(( v1.begin<variant::Int32, variant::Double>() ), variant_error);

    // Runtime error if we specify the wrong column variant type
    BOOST_CHECK_THROW(( v1.begin<variant::Int32, variant::List>() ), variant_error);

    data_table<variant::Int32, variant::Tuple>::const_iterator typed_iter = v1.begin<variant::Int32, variant::Tuple>();
    data_table<variant::Int32, variant::Tuple>::const_iterator typed_end  = v1.end<variant::Int32, variant::Tuple>();

    BOOST_CHECK(typed_iter != typed_end);
    BOOST_CHECK(typed_iter->get<1>() == t1);
    ++typed_iter;
    BOOST_CHECK(typed_iter->get<1>() == t2);
    ++typed_iter;
    BOOST_CHECK(typed_iter == typed_end);

    variant::const_iterator variant_iter = v1.begin();
    variant::const_iterator variant_end = v1.end();

    BOOST_CHECK(variant_iter != variant_end);
    BOOST_CHECK(variant_iter->at(1) == t1);
    ++variant_iter;
    BOOST_CHECK(variant_iter->at(1) == t2);
    ++variant_iter;
    BOOST_CHECK(variant_iter == variant_end);
}

BOOST_AUTO_TEST_CASE(test_data_table_iterator)
{
    variant v1(variant::DataTable);
    v1.add_column(variant::DateTime)
      .add_column(variant::Int32)
      .add_column(variant::Double);

    // Or one can write: data_table<variant::DateTime, variant::Int32, variant::Double>::iterator empty_begin = ...
    data_table_iterator<variant::DateTime, variant::Int32, variant::Double>
        empty_begin = v1.begin<variant::DateTime, variant::Int32, variant::Double>();
    data_table_iterator<variant::DateTime, variant::Int32, variant::Double>
        empty_end = v1.end<variant::DateTime, variant::Int32, variant::Double>();

    BOOST_CHECK(empty_begin==empty_end);

    data_table_row<variant::DateTime, variant::Int32, variant::Double>::type
        row1 = boost::tuples::make_tuple(boost::posix_time::time_from_string("2002-01-20 09:00:00.000"), 42, 3.141);
    data_table_row<variant::DateTime, variant::Int32, variant::Double>::type
        row2 = boost::tuples::make_tuple(boost::posix_time::time_from_string("2002-01-20 10:00:00.000"), 666, 2.718);

    v1.push_back(row1)
      .push_back(row2);

    data_table_iterator<variant::DateTime, variant::Int32, variant::Double>
        begin =	v1.begin<variant::DateTime, variant::Int32, variant::Double>();
    data_table_iterator<variant::DateTime, variant::Int32, variant::Double>
        end = v1.end<variant::DateTime, variant::Int32, variant::Double>();

    BOOST_CHECK_EQUAL(begin->get<0>(), row1.get<0>());
    BOOST_CHECK_EQUAL(begin->get<1>(), row1.get<1>());
    BOOST_CHECK_EQUAL(begin->get<2>(), row1.get<2>());

    ++begin;

    BOOST_CHECK(begin!=end);

    BOOST_CHECK_EQUAL(begin->get<0>(), row2.get<0>());
    BOOST_CHECK_EQUAL(begin->get<1>(), row2.get<1>());
    BOOST_CHECK_EQUAL(begin->get<2>(), row2.get<2>());

    ++begin;

    BOOST_CHECK(begin==end);

    --begin;
    BOOST_CHECK(begin->get<1>()==666);

    begin->get<1>() = 1729;

    BOOST_CHECK(begin->get<1>()==1729);
}

BOOST_AUTO_TEST_CASE(test_data_table_variant_iterator)
{
    variant v1(variant::DataTable);

    BOOST_CHECK(v1.begin()==v1.end());

    v1.add_column(variant::DateTime)
      .add_column(variant::Int32)
      .add_column(variant::Double);

    BOOST_CHECK(v1.begin()==v1.end());

    // Or one can write: data_table<variant::DateTime, variant::Int32, variant::Double>::value_type row1 = ...
    data_table_row<variant::DateTime, variant::Int32, variant::Double>::type
        row1 = boost::tuples::make_tuple(boost::posix_time::time_from_string("2002-01-20 09:00:00.000"), 42, 3.141);
    data_table_row<variant::DateTime, variant::Int32, variant::Double>::type
        row2 = boost::tuples::make_tuple(boost::posix_time::time_from_string("2002-01-20 10:00:00.000"), 1, 2.718);

    v1.push_back(row1)
      .push_back(row2);

    BOOST_CHECK(v1.begin()!=v1.end());

    variant::iterator begin(v1.begin());
    variant::iterator end(v1.end());

    BOOST_CHECK_THROW(begin.key(), variant_error);
    BOOST_CHECK_THROW(begin.time(), variant_error);

    BOOST_CHECK(begin->is<variant::Tuple>());
    BOOST_CHECK(begin->at(0).is<variant::DateTime>());
    BOOST_CHECK(begin->at(1).is<variant::Int32>());
    BOOST_CHECK(begin->at(2).is<variant::Double>());

    data_table_row<variant::DateTime, variant::Int32, variant::Double>::type
        tuple1 = boost::tuples::make_tuple(
            begin->at(0).as<variant::date_time_t>(),
            begin->at(1).as<boost::int32_t>(),
            begin->at(2).as<double>()
        );
    BOOST_CHECK_EQUAL(row1.get<0>(), tuple1.get<0>());
    BOOST_CHECK_EQUAL(row1.get<1>(), tuple1.get<1>());
    BOOST_CHECK_EQUAL(row1.get<2>(), tuple1.get<2>());

    ++begin;

    data_table_row<variant::DateTime, variant::Int32, variant::Double>::type
        tuple2 = boost::tuples::make_tuple(
            begin->at(0).as<variant::date_time_t>(),
            begin->at(1).as<boost::int32_t>(),
            begin->at(2).as<double>()
        );
    BOOST_CHECK_EQUAL(row2.get<0>(), tuple2.get<0>());
    BOOST_CHECK_EQUAL(row2.get<1>(), tuple2.get<1>());
    BOOST_CHECK_EQUAL(row2.get<2>(), tuple2.get<2>());

    ++begin;

    BOOST_CHECK(begin==end);
}

BOOST_AUTO_TEST_CASE(test_data_table_large_number_of_columns)
{
    variant dt(variant::DataTable);

    #define NUM_COLUMNS 50

    for (int i = 0; i < NUM_COLUMNS; ++i)
        dt.add_column(variant::Int32);

    BOOST_CHECK_THROW(dt.add_column(variant::Int32), variant_error);

    data_table_row<
        BOOST_PP_ENUM(NUM_COLUMNS, IDENTITY, variant::Int32)
    >::type row =
        make_row(BOOST_PP_ENUM_PARAMS(NUM_COLUMNS, (int)));

    dt.push_back(row);

    data_table_const_iterator<
        BOOST_PP_ENUM(NUM_COLUMNS, IDENTITY, variant::Int32)
    > begin =
        dt.begin<
            BOOST_PP_ENUM(NUM_COLUMNS, IDENTITY, variant::Int32)
        >();

    #define CHECK_EQUAL_TUPLE_ELEMENT_N(z, n, _) \
        BOOST_CHECK_EQUAL(begin->get<n>(), boost::get<n>(row));

    BOOST_PP_REPEAT(NUM_COLUMNS, CHECK_EQUAL_TUPLE_ELEMENT_N, _)
}

template <typename Duration>
void duration_checkpoint(std::ostream& os, const std::string& type_name, const std::string& operation_name,
    const boost::chrono::high_resolution_clock::time_point& start, const boost::chrono::high_resolution_clock::time_point& finish)
{
    os << "[" << type_name << "] "
       << operation_name << " took "
       << Duration(boost::chrono::duration_cast<Duration>(finish - start).count()) << "." << std::endl;
}

BOOST_AUTO_TEST_CASE(test_data_table_binary_serialization_performance)
{
    /* // Start of commented-out performance test (uncomment to run)

    static const size_t rows = 1000000;

    boost::chrono::high_resolution_clock::time_point start;
    boost::chrono::high_resolution_clock::time_point finish;
    typedef boost::chrono::milliseconds duration_resolution;

    {
        variant ts(variant::TimeSeries);

        const boost::posix_time::ptime initial_time = boost::posix_time::time_from_string("2002-01-20 09:00:00.000");

        start = boost::chrono::high_resolution_clock::now();
        for (int i = 0; i < rows; ++i)
        {
            boost::posix_time::ptime datetime = initial_time + boost::posix_time::hours(i);
            variant value(variant::Tuple, 4);
            value.at(0) = variant(static_cast<double>(100.4 + i));
            value.at(1) = variant("value0");
            value.at(2) = variant(static_cast<boost::int32_t>(0 + i));
            value.at(3) = variant(static_cast<float>(1.25f + i));

            ts.push_back(datetime, value);
        }
        finish = boost::chrono::high_resolution_clock::now();

        duration_checkpoint<duration_resolution>(std::cout, "TimeSeries", "Construction", start, finish);

        std::ostringstream oss;
        binary_writer bw(oss);

        start = boost::chrono::high_resolution_clock::now();
        bw << ts;
        finish = boost::chrono::high_resolution_clock::now();

        duration_checkpoint<duration_resolution>(std::cout, "TimeSeries", "Serialization", start, finish);

        std::istringstream iss(oss.str());
        binary_reader br(iss);
        variant deserialized_ts;

        start = boost::chrono::high_resolution_clock::now();
        br >> deserialized_ts;
        finish = boost::chrono::high_resolution_clock::now();

        duration_checkpoint<duration_resolution>(std::cout, "TimeSeries", "Deserialization", start, finish);

        start = boost::chrono::high_resolution_clock::now();
    }
    finish = boost::chrono::high_resolution_clock::now();
    duration_checkpoint<duration_resolution>(std::cout, "TimeSeries", "Destruction", start, finish);

    {
        variant dt(variant::DataTable);
        dt.add_column(variant::DateTime, "Time")
          .add_column(variant::Double,   "Price")
          .add_column(variant::String,   "Data")
          .add_column(variant::Int32,    "Volume")
          .add_column(variant::Float,    "Col5");

        const boost::posix_time::ptime initial_time = boost::posix_time::time_from_string("2002-01-20 09:00:00.000");

        start = boost::chrono::high_resolution_clock::now();
        for (int i = 0; i < rows; ++i)
            dt.push_back(boost::tuples::make_tuple(
                initial_time + boost::posix_time::hours(i),
                100.4 + i,
                detail::string("value0"),
                0 + i,
                1.25f + i
            ));
        finish = boost::chrono::high_resolution_clock::now();

        duration_checkpoint<duration_resolution>(std::cout, "DataTable", "Construction", start, finish);

        std::ostringstream oss;
        binary_writer bw(oss);

        start = boost::chrono::high_resolution_clock::now();
        bw << dt;
        finish = boost::chrono::high_resolution_clock::now();

        duration_checkpoint<duration_resolution>(std::cout, "DataTable", "Serialization", start, finish);

        std::istringstream iss(oss.str());
        binary_reader br(iss);
        variant deserialized_dt;

        start = boost::chrono::high_resolution_clock::now();
        br >> deserialized_dt;
        finish = boost::chrono::high_resolution_clock::now();

        duration_checkpoint<duration_resolution>(std::cout, "DataTable", "Deserialization", start, finish);

        start = boost::chrono::high_resolution_clock::now();
    }
    finish = boost::chrono::high_resolution_clock::now();
    duration_checkpoint<duration_resolution>(std::cout, "DataTable", "Destruction", start, finish);

    */ // End of commented-out performance test (uncomment to run)
}

BOOST_AUTO_TEST_SUITE_END()
