//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>  

#include <boost/math/special_functions/fpclassify.hpp>

using boost::unit_test::test_suite;    

#include <protean/variant.hpp>
#include <protean/xml_reader.hpp>
#include <protean/xml_writer.hpp>
#include <protean/object_factory.hpp>
using namespace protean;

struct XmlFixture
{
    XmlFixture()  { xml_reader::initialiseXerces(); }
    ~XmlFixture() { xml_reader::shutdownXerces(); }
};

BOOST_FIXTURE_TEST_SUITE(xml_streams_suite, XmlFixture)

BOOST_AUTO_TEST_CASE(test_xml_primitives)
{
    boost::int32_t arg_int32(0xffffffff);
    boost::uint32_t arg_uint32(0xffffffff);
    boost::int64_t arg_int64(0xffffffffffffffffll);
    boost::uint64_t arg_uint64(0xffffffffffffffffull);
    bool arg_bool(true);
    float arg_float(std::numeric_limits<float>::max());
    double arg_double(std::numeric_limits<double>::max());
    std::string arg_string("test string");
    
    variant v1(variant::Dictionary);
    v1.insert("Int32", variant(arg_int32))
      .insert("UInt32", variant(arg_uint32))
      .insert("Int64", variant(arg_int64))
      .insert("UInt64", variant(arg_uint64))
      .insert("Float", variant(arg_float))
      .insert("Double", variant(arg_double))
      .insert("Boolean", variant(arg_bool))
      .insert("String", variant(arg_string))
      .insert("None", variant(variant::None));

    std::ostringstream oss;
    xml_writer writer(oss, xml_mode::Indent);
    writer << v1;

    variant v2;
    std::stringstream iss;
    iss << oss.str();
    xml_reader reader(iss);
    reader >> v2;

    BOOST_CHECK(v1["Int32"].is<variant::Int32>());
    BOOST_CHECK(v1["UInt32"].is<variant::UInt32>());
    BOOST_CHECK(v1["Int64"].is<variant::Int64>());
    BOOST_CHECK(v1["UInt64"].is<variant::UInt64>());
    BOOST_CHECK(v1["Float"].is<variant::Float>());
    BOOST_CHECK(v1["Double"].is<variant::Double>());
    BOOST_CHECK(v1["Boolean"].is<variant::Boolean>());
    BOOST_CHECK(v1["String"].is<variant::String>());
    BOOST_CHECK(v1["None"].is<variant::None>());

    BOOST_CHECK_EQUAL(v1["Int32"].as<boost::int32_t>(), arg_int32);
    BOOST_CHECK_EQUAL(v1["UInt32"].as<boost::uint32_t>(), arg_uint32);
    BOOST_CHECK_EQUAL(v1["Int64"].as<boost::int64_t>(), arg_int64);
    BOOST_CHECK_EQUAL(v1["UInt64"].as<boost::uint64_t>(), arg_uint64);
    BOOST_CHECK_EQUAL(v1["Float"].as<float>(), arg_float);
    BOOST_CHECK_EQUAL(v1["Double"].as<double>(), arg_double);
    BOOST_CHECK_EQUAL(v1["Boolean"].as<bool>(), arg_bool);
    BOOST_CHECK_EQUAL(v1["String"].as<std::string>(), arg_string);
}

BOOST_AUTO_TEST_CASE(test_xml_exception)
{
    exception_data arg("test_exception", "test message");
    variant v1(arg);

    std::ostringstream oss;
    xml_writer writer(oss, xml_mode::Indent);
    writer << v1;

    variant v2;
    std::stringstream iss;
    iss << oss.str();
    xml_reader reader(iss);
    reader >> v2;

    BOOST_CHECK(v1.compare(v2)==0);
}

BOOST_AUTO_TEST_CASE(test_xml_buffer)
{
    char arg [] = "some buffer contents";
    variant v1(static_cast<void*>(arg), strlen(arg));

    std::ostringstream oss;
    xml_writer writer(oss);
    writer << v1;

    variant v2;
    std::stringstream iss;
    iss << oss.str();
    xml_reader reader(iss);
    reader >> v2;

    BOOST_CHECK(v1.compare(v2)==0);
}

BOOST_AUTO_TEST_CASE(test_xml_collection)
{
    {
        variant v1(variant::List);
        v1.push_back(variant("string"));
        v1.push_back(variant(1.0));

        std::ostringstream oss;
        xml_writer writer(oss);
        writer << v1;

        variant v2;
        std::stringstream iss;
        iss << oss.str();
        xml_reader reader(iss);
        reader >> v2;

        BOOST_CHECK(v1.compare(v2)==0);
    }

    {
        variant v1(variant::Tuple, 2);
        v1[0] = "string";
        v1[1] = 1.0;

        std::ostringstream oss;
        xml_writer writer(oss);
        writer << v1;

        variant v2;
        std::stringstream iss;
        iss << oss.str();
        xml_reader reader(iss);
        reader >> v2;

        BOOST_CHECK(v1.compare(v2)==0);
    }

    {
        variant v1(variant::Dictionary);
        v1.insert("child1", variant("string"));
        v1.insert("child2", variant(1.0));

        std::ostringstream oss;
        xml_writer writer(oss);
        writer << v1;

        variant v2;
        std::stringstream iss;
        iss << oss.str();
        xml_reader reader(iss);
        reader >> v2;

        BOOST_CHECK(v1.compare(v2)==0);
    }

    {
        variant v1(variant::Bag);
        v1.insert("child1", variant("string"));
        v1.insert("child2", variant(1.0));

        std::ostringstream oss;
        xml_writer writer(oss);
        writer << v1;

        variant v2;
        std::stringstream iss;
        iss << oss.str();
        xml_reader reader(iss);
        reader >> v2;

        BOOST_CHECK(v1.compare(v2)==0);
    }

    {
        variant v1(variant::TimeSeries);

        variant::date_t date1 = variant::date_t(2007, 1, 3);
        variant::time_t time1 = variant::time_t(10, 30, 0);
        variant::date_t date2 = variant::date_t(2007, 1, 4);
        variant::time_t time2 = variant::time_t(11, 30, 0);

        v1.push_back(variant::date_time_t(date1, time1), variant("string"));
        v1.push_back(variant::date_time_t(date2, time2), variant(1.0));

        std::ostringstream oss;
        xml_writer writer(oss);
        writer << v1;

        variant v2;
        std::stringstream iss;
        iss << oss.str();
        xml_reader reader(iss);
        reader >> v2;

        BOOST_CHECK(v1.compare(v2)==0);
    }
}

class testing_object : public object
{
public:
    testing_object() :
        m_count(++sm_count)
    {}
    testing_object(const std::string& id) :
        m_count(++sm_count),
        m_id(id)
    {}
    testing_object(const testing_object& rhs) :
        m_count(++sm_count),
        m_id(rhs.m_id)
    {}

    boost::int32_t count() const {
        return m_count;
    }

    std::string id() const {
        return m_id;
    }

    void set_id(const std::string& id) {
        m_id = id;
    }

public:
    handle<object> clone() const
    {
        return handle<object>(new testing_object(*this));
    }
    int version() const
    {
        return 1;
    }
    void deflate(variant& params) const
    {
        params = variant(variant::Dictionary);
        params.insert("count",  variant(m_count));
        params.insert("id",     variant(m_id));
    }
    void inflate(const variant& params, int version)
    {
        m_count = params["count"].as<boost::int32_t>();
        m_id = params["id"].as<std::string>();
    }
    /*virtual*/ std::string name() const {
       return "protean::xml_streams::testing_object";
    }

private:
    static boost::int32_t  sm_count;
    boost::int32_t  m_count;
    std::string     m_id;
};

boost::int32_t testing_object::sm_count = 0;

BOOST_AUTO_TEST_CASE(test_xml_object)
{
    testing_object arg("test object");
    variant v1(arg);

    std::ostringstream oss;
    xml_writer writer(oss);
    writer << v1;

    variant v2;
    std::stringstream iss1;
    iss1 << oss.str();
    xml_reader reader1(iss1);
    reader1 >> v2;

    BOOST_CHECK(v2.is<object_proxy>());
    BOOST_CHECK(v1.compare(v2)==0);

    variant v3;
    std::stringstream iss2;
    iss2 << oss.str();

    object_factory factory;
    factory.register_object<testing_object>();
    xml_reader reader2(iss2);
    reader2.set_factory(factory);
    reader2 >> v3;

    BOOST_CHECK(!v3.is<object_proxy>());
    BOOST_CHECK(v1.compare(v3)==0);
}

BOOST_AUTO_TEST_CASE(test_xml_untyped)
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
    BOOST_CHECK_EQUAL(v1["Int32"].as<boost::int32_t>(), static_cast<int> (0xFFFFFFFF));
    BOOST_CHECK_EQUAL(v1["UInt32"].as<boost::uint32_t>(), 0xFFFFFFFF);
    BOOST_CHECK_EQUAL(v1["Int64"].as<boost::int64_t>(), static_cast<long long>(0xFFFFFFFFFFFFFFFFll));
    BOOST_CHECK_EQUAL(v1["UInt64"].as<boost::uint64_t>(), 0xFFFFFFFFFFFFFFFFull);
    BOOST_CHECK_EQUAL(v1["Float"].as<float>(), 4.0);
    BOOST_CHECK_EQUAL(v1["Double"].as<double>(), 3.7);
    
    variant::date_t date = variant::date_t(2007, 1, 3);
    BOOST_CHECK_EQUAL(v1["Date"].as<variant::date_t>(), date);
    variant::time_t time = variant::time_t(10, 30, 0);
    BOOST_CHECK_EQUAL(v1["Time"].as<variant::time_t>(), time);
    variant::date_time_t date_time = variant::date_time_t(date, time);
    BOOST_CHECK_EQUAL(v1["DateTime"].as<variant::date_time_t>(), date_time);

    std::stringstream oss;
    xml_writer writer(oss, xml_mode::Preserve | xml_mode::Indent);
    writer << variant(variant::Dictionary).insert("Variant", v1);

    BOOST_CHECK_EQUAL(iss.str(), oss.str());
}

BOOST_AUTO_TEST_CASE(test_xml_validation)
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

BOOST_AUTO_TEST_CASE(test_xml_simple_array)
{
    typed_array a1(3, variant::String);
    a1[0] = "s1";
    a1[1] = "s2";
    a1[2] = "s3";

    variant v1(a1);

    std::stringstream ss;
    xml_writer writer(ss);
    writer << v1;

    variant v2;
    xml_reader reader(ss);
    reader >> v2;

    BOOST_CHECK(v1.compare(v2)==0);
}

BOOST_AUTO_TEST_CASE(test_xml_complex_array)
{
    typed_array a1(3, variant::Dictionary);
    a1[0] = variant(variant::Dictionary).insert("a", variant(1)).insert("b", variant(2));
    a1[1] = variant(variant::Dictionary).insert("b", variant(3)).insert("c", variant(4));
    a1[2] = variant(variant::Dictionary).insert("d", variant(5)).insert("e", variant(6));

    variant v1(a1);

    std::stringstream ss;
    xml_writer writer(ss);
    writer << v1;

    variant v2;
    xml_reader reader(ss);
    reader >> v2;

    BOOST_CHECK(v1.compare(v2)==0);
}

BOOST_AUTO_TEST_CASE(test_xml_preserve)
{
    static const std::string xml =
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<Variant aBoolean=\"true\" aDate=\"2007-01-03\" aDateTime=\"2007-01-03T10:30:00\" aFloat=\"0.5\" aDouble=\"3.9\" aInteger=\"1\" aUnsigned=\"2\" aInt32=\"3\" aUInt32=\"4\" aInt64=\"5\" aUInt64=\"6\" aString=\"Element\" aTime=\"10:30:00\">\n"
        "  <!-- A comment -->\n"
        "  <Boolean>true</Boolean>\n"
        "  <Date>2007-01-03</Date>\n"
        "  <DateTime>2007-01-03T10:30:00</DateTime>\n"
        "  <Float>0.5</Float>\n"
        "  <!-- Another comment -->\n"
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
    xml_reader reader2(iss, xml_mode::Preserve);
    reader2 >> v1;

    std::ostringstream oss;
    xml_writer writer(oss, xml_mode::Preserve);
    writer << v1;

    BOOST_CHECK_EQUAL(iss.str(), oss.str());
}

BOOST_AUTO_TEST_CASE(test_xml_preserve_ws)
{
    static const std::string xml =
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<?target data=\"test\"?>\n"
        "<elem1>\n"
        "    1 2\n"
        "    <elem2/>\n"
        "         3\n"
        "    <elem3/>\n"
        "\n"
        "  4\n"
        "\n"
        "       6\n"
        "</elem1>";

    std::stringstream iss;
    iss << xml;

    variant v1;
    xml_reader reader(iss, xml_mode::Preserve);
    reader >> v1;

    std::ostringstream oss;
    xml_writer writer(oss, xml_mode::Preserve);
    writer << v1;

    BOOST_CHECK_EQUAL(iss.str(), oss.str());
}

BOOST_AUTO_TEST_CASE(test_xml_indent)
{
    static const std::string xml =
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<?target data=\"test\"?><elem1><elem2>1</elem2>\n <elem3/> </elem1>";

    static const std::string formatted_xml =
        "<?target data=\"test\"?>\n"
        "<elem1>\n"
        "  <elem2>1</elem2>\n"
        "  <elem3/>\n"
        "</elem1>";

    std::stringstream iss;
    iss << xml;

    variant v1;
    xml_reader reader(iss, xml_mode::Preserve | xml_mode::StripSpace);
    reader >> v1;

    std::ostringstream oss;
    xml_writer writer(oss, xml_mode::Preserve | xml_mode::Indent | xml_mode::NoHeader);
    writer << v1;

    BOOST_CHECK_EQUAL(formatted_xml, oss.str());
}

BOOST_AUTO_TEST_CASE(test_xml_double)
{
    static const std::string xml =
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<Variant variant=\"List\">\n"
        "  <Variant variant=\"Double\">NaN</Variant>\n"
        "  <Variant variant=\"Double\">-INF</Variant>\n"
        "  <Variant variant=\"Double\">INF</Variant>\n"
        "  <Variant variant=\"Any\">NaN</Variant>\n"
        "  <Variant variant=\"Any\">-INF</Variant>\n"
        "  <Variant variant=\"Any\">INF</Variant>\n"
        "</Variant>";

    std::stringstream iss;
    iss << xml;

    variant v1;
    xml_reader reader(iss);
    reader >> v1;

    BOOST_REQUIRE_EQUAL(v1.size(), 6u);
    BOOST_CHECK(boost::math::fpclassify(v1[0].as<double>())==FP_NAN);
    BOOST_CHECK(boost::math::fpclassify(v1[1].as<double>())==FP_INFINITE && v1[1].as<double>()<.0);
    BOOST_CHECK(boost::math::fpclassify(v1[2].as<double>())==FP_INFINITE && v1[2].as<double>()>.0);
    BOOST_CHECK(boost::math::fpclassify(v1[3].as<double>())==FP_NAN);
    BOOST_CHECK(boost::math::fpclassify(v1[4].as<double>())==FP_INFINITE && v1[4].as<double>()<.0);
    BOOST_CHECK(boost::math::fpclassify(v1[5].as<double>())==FP_INFINITE && v1[5].as<double>()>.0);

    std::ostringstream oss;
    xml_writer writer(oss, xml_mode::Indent);
    writer << v1;

    BOOST_CHECK_EQUAL(iss.str(), oss.str());
}

BOOST_AUTO_TEST_CASE(test_xml_float)
{
    static const std::string xml =
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<Variant variant=\"List\">\n"
        "  <Variant variant=\"Float\">NaN</Variant>\n"
        "  <Variant variant=\"Float\">-INF</Variant>\n"
        "  <Variant variant=\"Float\">INF</Variant>\n"
        "  <Variant variant=\"Any\">NaN</Variant>\n"
        "  <Variant variant=\"Any\">-INF</Variant>\n"
        "  <Variant variant=\"Any\">INF</Variant>\n"
        "</Variant>";

    std::stringstream iss;
    iss << xml;

    variant v1;
    xml_reader reader(iss);
    reader >> v1;

    BOOST_REQUIRE_EQUAL(v1.size(), 6u);
    BOOST_CHECK(boost::math::fpclassify(v1[0].as<float>())==FP_NAN);
    BOOST_CHECK(boost::math::fpclassify(v1[1].as<float>())==FP_INFINITE && v1[1].as<float>()<.0);
    BOOST_CHECK(boost::math::fpclassify(v1[2].as<float>())==FP_INFINITE && v1[2].as<float>()>.0);
    BOOST_CHECK(boost::math::fpclassify(v1[3].as<float>())==FP_NAN);
    BOOST_CHECK(boost::math::fpclassify(v1[4].as<float>())==FP_INFINITE && v1[4].as<float>()<.0);
    BOOST_CHECK(boost::math::fpclassify(v1[5].as<float>())==FP_INFINITE && v1[5].as<float>()>.0);

    std::ostringstream oss;
    xml_writer writer(oss, xml_mode::Indent);
    writer << v1;

    BOOST_CHECK_EQUAL(iss.str(), oss.str());
}

BOOST_AUTO_TEST_SUITE_END()
