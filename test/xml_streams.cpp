//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>  

using boost::unit_test::test_suite;    

#include <protean/variant.hpp>
#include <protean/xml_reader.hpp>
#include <protean/xml_writer.hpp>
using namespace protean;

static const std::string typed_xml =
    "<Variant variant=\"Dictionary\">"
    "    <Any variant=\"Any\">Any Variant</Any>"
    "    <Boolean variant=\"Boolean\">true</Boolean>"
    "    <Date variant=\"Date\">2007-01-03</Date>"
    "    <DateTime variant=\"DateTime\">2007-01-03T10:30:00</DateTime>"
    "    <Double variant=\"Double\">3.7000000000000002</Double>"
    "    <Float variant=\"Float\">4</Float>"
    "    <Int32 variant=\"Int32\">-1</Int32>"
    "    <Int64 variant=\"Int64\">-1</Int64>"
    "    <String variant=\"String\">Element</String>"
    "    <Time variant=\"Time\">10:30:00</Time>"
    "    <UInt32 variant=\"UInt32\">4294967295</UInt32>"
    "    <UInt64 variant=\"UInt64\">18446744073709551615</UInt64>"
    "</Variant>";

void test_typed_xml()
{
    std::stringstream iss;
    iss << typed_xml;

    variant v1;
    xml_reader reader(iss);
    reader >> v1;

    BOOST_CHECK(v1.is<variant::Dictionary>());
    BOOST_CHECK(v1["String"].is<std::string>());
    BOOST_CHECK(v1["Boolean"].is<bool>());
    BOOST_CHECK(v1["Int32"].is<variant::int32_t>());
    BOOST_CHECK(v1["UInt32"].is<variant::uint32_t>());
    BOOST_CHECK(v1["Int64"].is<variant::int64_t>());
    BOOST_CHECK(v1["UInt64"].is<variant::uint64_t>());
    BOOST_CHECK(v1["Float"].is<float>());
    BOOST_CHECK(v1["Double"].is<double>());
    BOOST_CHECK(v1["Date"].is<variant::date_t>());
    BOOST_CHECK(v1["Time"].is<variant::time_t>());
    BOOST_CHECK(v1["DateTime"].is<variant::date_time_t>());

    BOOST_CHECK_EQUAL(v1["String"].as<std::string>(), "Element");
    BOOST_CHECK_EQUAL(v1["Boolean"].as<bool>(), true);
    BOOST_CHECK_EQUAL(v1["Int32"].as<variant::int32_t>(), 0xFFFFFFFF);
    BOOST_CHECK_EQUAL(v1["UInt32"].as<variant::uint32_t>(), 0xFFFFFFFF);
    BOOST_CHECK_EQUAL(v1["Int64"].as<variant::int64_t>(), 0xFFFFFFFFFFFFFFFF);
    BOOST_CHECK_EQUAL(v1["UInt64"].as<variant::uint64_t>(), 0xFFFFFFFFFFFFFFFF);
    BOOST_CHECK_EQUAL(v1["Float"].as<float>(), 4.0);
    BOOST_CHECK_EQUAL(v1["Double"].as<double>(), 3.7);
    
    variant::date_t date = variant::date_t(2007, 1, 3);
    BOOST_CHECK_EQUAL(v1["Date"].as<variant::date_t>(), date);
    variant::time_t time = variant::time_t(10, 30, 0);
    BOOST_CHECK_EQUAL(v1["Time"].as<variant::time_t>(), time);
    variant::date_time_t date_time = variant::date_time_t(date, time);
    BOOST_CHECK_EQUAL(v1["DateTime"].as<variant::date_time_t>(), date_time);

    std::stringstream oss;
    xml_writer writer(oss);
    writer << v1;

    BOOST_CHECK_EQUAL(iss.str(), oss.str());
}

test_suite* init_unit_test_suite(int, char* []) 
{
    test_suite* test = BOOST_TEST_SUITE("object type");
    test->add(BOOST_TEST_CASE(&test_typed_xml));

    return test;
}
