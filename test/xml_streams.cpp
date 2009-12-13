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



void test_xml_typed()
{
    static const std::string xml =
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<Variant variant=\"Dictionary\">\n"
        "  <Boolean variant=\"Boolean\">true</Boolean>\n"
        "  <Date variant=\"Date\">2007-01-03</Date>\n"
        "  <DateTime variant=\"DateTime\">2007-01-03T10:30:00</DateTime>\n"
        "  <Double variant=\"Double\">3.7000000000000002</Double>\n"
        "  <Float variant=\"Float\">4</Float>\n"
        "  <Int32 variant=\"Int32\">-1</Int32>\n"
        "  <Int64 variant=\"Int64\">-1</Int64>\n"
        "  <String variant=\"String\">Element</String>\n"
        "  <Time variant=\"Time\">10:30:00</Time>\n"
        "  <UInt32 variant=\"UInt32\">4294967295</UInt32>\n"
        "  <UInt64 variant=\"UInt64\">18446744073709551615</UInt64>\n"
        "</Variant>";

    std::stringstream iss;
    iss << xml;

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



void test_xml_untyped()
{
    static const std::string xml =
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<Variant>\n"
        "  <Boolean>true</Boolean>\n"
        "  <Date>2007-01-03</Date>\n"
        "  <DateTime>2007-01-03T10:30:00</DateTime>\n"
        "  <Double>3.7000000000000002</Double>\n"
        "  <Float>4</Float>\n"
        "  <Int32>-1</Int32>\n"
        "  <Int64>-1</Int64>\n"
        "  <String>Element</String>\n"
        "  <Time>10:30:00</Time>\n"
        "  <UInt32>4294967295</UInt32>\n"
        "  <UInt64>18446744073709551615</UInt64>\n"
        "</Variant>";

    std::stringstream iss;
    iss << xml;

    variant v1;
    xml_reader reader(iss);
    reader >> v1;

    BOOST_CHECK(v1.is<variant::Bag>());
    BOOST_CHECK(v1["String"].is<variant::Any>());
    BOOST_CHECK(v1["Boolean"].is<variant::Any>());
    BOOST_CHECK(v1["Int32"].is<variant::Any>());
    BOOST_CHECK(v1["UInt32"].is<variant::Any>());
    BOOST_CHECK(v1["Int64"].is<variant::Any>());
    BOOST_CHECK(v1["UInt64"].is<variant::Any>());
    BOOST_CHECK(v1["Float"].is<variant::Any>());
    BOOST_CHECK(v1["Double"].is<variant::Any>());
    BOOST_CHECK(v1["Date"].is<variant::Any>());
    BOOST_CHECK(v1["Time"].is<variant::Any>());
    BOOST_CHECK(v1["DateTime"].is<variant::Any>());

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
    xml_writer writer(oss, xml_writer::Preserve);
    writer << variant(variant::Dictionary).insert("Variant", v1);

    BOOST_CHECK_EQUAL(iss.str(), oss.str());
}


void test_xml_validation()
{
    static const std::string xml =
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<Variant aBoolean=\"true\" aDate=\"2007-01-03\" aDateTime=\"2007-01-03T10:30:00\" aFloat=\"0.5\" aDouble=\"3.9\" aInteger=\"1\" aUnsigned=\"2\" aInt32=\"3\" aUInt32=\"4\" aInt64=\"5\" aUInt64=\"6\" aString=\"Element\" aTime=\"10:30:00\">\n"
        "  <Boolean>true</Boolean>\n"
        "  <Date>2007-01-03</Date>\n"
        "  <DateTime>2007-01-03T10:30:00</DateTime>\n"
        "  <Float>0.5</Float>\n"
        "  <Double>1</Double>\n"
        "  <Integer>1</Integer>\n"
        "  <Unsigned>2</Unsigned>\n"
        "  <Int32>3</Int32>\n"
        "  <UInt32>4</UInt32>\n"
        "  <Int64>5</Int64>\n"
        "  <UInt64>6</UInt64>\n"
        "  <String>Element</String>\n"
        "  <Time>10:30:00</Time>\n"
        "</Variant>";

    static const std::string xdr =
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<xsd:schema xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\n"
        "  <xsd:element name=\"Variant\">\n"
        "    <xsd:complexType mixed=\"true\">\n"
        "      <xsd:all>\n"
        "        <xsd:element name=\"String\"   type=\"xsd:string\"/>\n"
        "        <xsd:element name=\"Boolean\"  type=\"xsd:boolean\"/>\n"
        "        <xsd:element name=\"Integer\"  type=\"xsd:integer\"/>\n"
        "        <xsd:element name=\"Unsigned\" type=\"xsd:positiveInteger\"/>\n"
        "        <xsd:element name=\"Int32\"    type=\"xsd:int\"/>\n"
        "        <xsd:element name=\"UInt32\"   type=\"xsd:unsignedInt\"/>\n"
        "        <xsd:element name=\"Int64\"    type=\"xsd:long\"/>\n"
        "        <xsd:element name=\"UInt64\"   type=\"xsd:unsignedLong\"/>\n"
        "        <xsd:element name=\"Float\"    type=\"xsd:float\"/>\n"
        "        <xsd:element name=\"Double\"   type=\"xsd:double\"/>\n"
        "        <xsd:element name=\"Date\"     type=\"xsd:date\"/>\n"
        "        <xsd:element name=\"Time\"     type=\"xsd:time\"/>\n"
        "        <xsd:element name=\"DateTime\" type=\"xsd:dateTime\"/>\n"
        "      </xsd:all>\n"
        "      <xsd:attribute name=\"variant\"   type=\"xsd:string\" use=\"optional\" default=\"Dictionary\"/>\n"
        "      <xsd:attribute name=\"aString\"   type=\"xsd:string\"/>\n"
        "      <xsd:attribute name=\"aBoolean\"  type=\"xsd:boolean\"/>\n"
        "      <xsd:attribute name=\"aInteger\"  type=\"xsd:integer\"/>\n"
        "      <xsd:attribute name=\"aUnsigned\" type=\"xsd:positiveInteger\"/>\n"
        "      <xsd:attribute name=\"aInt32\"    type=\"xsd:int\"/>\n"
        "      <xsd:attribute name=\"aUInt32\"   type=\"xsd:unsignedInt\"/>\n"
        "      <xsd:attribute name=\"aInt64\"    type=\"xsd:long\"/>\n"
        "      <xsd:attribute name=\"aUInt64\"   type=\"xsd:unsignedLong\"/>\n"
        "      <xsd:attribute name=\"aFloat\"    type=\"xsd:float\"/>\n"
        "      <xsd:attribute name=\"aDouble\"   type=\"xsd:double\"/>\n"
        "      <xsd:attribute name=\"aDate\"     type=\"xsd:date\"/>\n"
        "      <xsd:attribute name=\"aTime\"     type=\"xsd:time\"/>\n"
        "      <xsd:attribute name=\"aDateTime\" type=\"xsd:dateTime\"/>\n"
        "    </xsd:complexType>\n"
        "  </xsd:element>\n"
        "</xsd:schema>\n";

    std::stringstream iss;
    iss << xml;

    std::stringstream xdr_ss;
    xdr_ss << xdr;

    variant v1;
    xml_reader reader1(iss);
    reader1.set_external_schema("my_schema");
	reader1.add_entity_stream("my_schema", xdr_ss);

    reader1 >> v1;

    BOOST_CHECK(v1.is<variant::Dictionary>());

    // Check node types
    BOOST_CHECK(v1["String"].is<variant::String>());
    BOOST_CHECK(v1["Boolean"].is<variant::Boolean>());
    BOOST_CHECK(v1["Integer"].is<variant::Int32>());
    BOOST_CHECK(v1["Unsigned"].is<variant::UInt32>());
    BOOST_CHECK(v1["Int32"].is<variant::Int32>());
    BOOST_CHECK(v1["UInt32"].is<variant::UInt32>());
    BOOST_CHECK(v1["Int64"].is<variant::Int64>());
    BOOST_CHECK(v1["UInt64"].is<variant::UInt64>());
    BOOST_CHECK(v1["Float"].is<variant::Float>());
    BOOST_CHECK(v1["Double"].is<variant::Double>());
    BOOST_CHECK(v1["Date"].is<variant::Date>());
    BOOST_CHECK(v1["Time"].is<variant::Time>());
    BOOST_CHECK(v1["DateTime"].is<variant::DateTime>());

    // check attribute types
    BOOST_CHECK(v1["aString"].is<variant::String>());
    BOOST_CHECK(v1["aBoolean"].is<variant::Boolean>());
    BOOST_CHECK(v1["aInteger"].is<variant::Int32>());
    BOOST_CHECK(v1["aUnsigned"].is<variant::UInt32>());
    BOOST_CHECK(v1["aInt32"].is<variant::Int32>());
    BOOST_CHECK(v1["aUInt32"].is<variant::UInt32>());
    BOOST_CHECK(v1["aInt64"].is<variant::Int64>());
    BOOST_CHECK(v1["aUInt64"].is<variant::UInt64>());
    BOOST_CHECK(v1["aFloat"].is<variant::Float>());
    BOOST_CHECK(v1["aDouble"].is<variant::Double>());
    BOOST_CHECK(v1["aDate"].is<variant::Date>());
    BOOST_CHECK(v1["aTime"].is<variant::Time>());
    BOOST_CHECK(v1["aDateTime"].is<variant::DateTime>());
}

void test_xml_preserve()
{
    static const std::string xml =
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<Variant aBoolean=\"true\" aDate=\"2007-01-03\" aDateTime=\"2007-01-03T10:30:00\" aFloat=\"0.5\" aDouble=\"3.9\" aInteger=\"1\" aUnsigned=\"2\" aInt32=\"3\" aUInt32=\"4\" aInt64=\"5\" aUInt64=\"6\" aString=\"Element\" aTime=\"10:30:00\">\n"
        "  <Boolean>true</Boolean>\n"
        "  <Date>2007-01-03</Date>\n"
        "  <DateTime>2007-01-03T10:30:00</DateTime>\n"
        "  <Float>0.5</Float>\n"
        "  <Double>1</Double>\n"
        "  <Integer>1</Integer>\n"
        "  <Unsigned>2</Unsigned>\n"
        "  <Int32>3</Int32>\n"
        "  <UInt32>4</UInt32>\n"
        "  <Int64>5</Int64>\n"
        "  <UInt64>6</UInt64>\n"
        "  <String>Element</String>\n"
        "  <Time>10:30:00</Time>\n"
        "</Variant>";

    std::stringstream iss;
    iss << xml;

    variant v1;
    xml_reader reader2(iss, xml_reader::Preserve);
    reader2 >> v1;

    std::ostringstream oss;
    xml_writer writer(oss, xml_writer::Preserve);
    writer << v1;

    BOOST_CHECK_EQUAL(iss.str(), oss.str());
}

test_suite* init_unit_test_suite(int, char* []) 
{
    test_suite* test = BOOST_TEST_SUITE("object type");
    test->add(BOOST_TEST_CASE(&test_xml_typed));
    test->add(BOOST_TEST_CASE(&test_xml_untyped));
    test->add(BOOST_TEST_CASE(&test_xml_validation));
    test->add(BOOST_TEST_CASE(&test_xml_preserve));    

    return test;
}
