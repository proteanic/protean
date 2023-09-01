#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>  

using boost::unit_test::test_suite;    

#include <protean/variant.hpp>
#include <protean/binary_reader.hpp>
#include <protean/binary_writer.hpp>
#include <protean/object_factory.hpp>
using namespace protean;

BOOST_AUTO_TEST_SUITE(binary_streams_suite);

BOOST_AUTO_TEST_CASE(test_binary_primitives)
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
    binary_writer writer(oss);
    writer << v1;

    variant v2;
    std::stringstream iss;
    iss << oss.str();
    binary_reader reader(iss);
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

BOOST_AUTO_TEST_CASE(test_binary_primitives_zlib)
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
    binary_writer writer(oss, binary_mode::Compress);
    writer << v1;

    variant v2;
    std::stringstream iss;
    iss << oss.str();
    binary_reader reader(iss);
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

BOOST_AUTO_TEST_CASE(test_binary_exception)
{
    exception_data arg("test_exception", "test message");
    variant v1(arg);

    std::ostringstream oss;
    binary_writer writer(oss);
    writer << v1;

    variant v2;
    std::stringstream iss;
    iss << oss.str();
    binary_reader reader(iss);
    reader >> v2;

    BOOST_CHECK(v1.compare(v2)==0);
}

BOOST_AUTO_TEST_CASE(test_binary_buffer)
{
    char arg [] = "some buffer contents";
    variant v1(static_cast<void*>(arg), strlen(arg));

    std::ostringstream oss;
    binary_writer writer(oss);
    writer << v1;

    variant v2;
    std::stringstream iss;
    iss << oss.str();
    binary_reader reader(iss);
    reader >> v2;

    BOOST_CHECK(v1.compare(v2)==0);
}

BOOST_AUTO_TEST_CASE(test_binary_collection)
{
    {
        variant v1(variant::List);
        v1.push_back(variant("string"));
        v1.push_back(variant(1.0));

        std::ostringstream oss;
        binary_writer writer(oss);
        writer << v1;

        variant v2;
        std::stringstream iss;
        iss << oss.str();
        binary_reader reader(iss);
        reader >> v2;

        BOOST_CHECK(v1.compare(v2)==0);
    }

    {
        variant v1(variant::Tuple, 2);
        v1[0] = "string";
        v1[1] = 1.0;

        std::ostringstream oss;
        binary_writer writer(oss);
        writer << v1;

        variant v2;
        std::stringstream iss;
        iss << oss.str();
        binary_reader reader(iss);
        reader >> v2;

        BOOST_CHECK(v1.compare(v2)==0);
    }

    {
        variant v1(variant::Dictionary);
        v1.insert("child1", variant("string"));
        v1.insert("child2", variant(1.0));

        std::ostringstream oss;
        binary_writer writer(oss);
        writer << v1;

        variant v2;
        std::stringstream iss;
        iss << oss.str();
        binary_reader reader(iss);
        reader >> v2;

        BOOST_CHECK(v1.compare(v2)==0);
    }

    {
        variant v1(variant::Bag);
        v1.insert("child1", variant("string"));
        v1.insert("child2", variant(1.0));

        std::ostringstream oss;
        binary_writer writer(oss);
        writer << v1;

        variant v2;
        std::stringstream iss;
        iss << oss.str();
        binary_reader reader(iss);
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
        binary_writer writer(oss);
        writer << v1;

        variant v2;
        std::stringstream iss;
        iss << oss.str();
        binary_reader reader(iss);
        reader >> v2;

        BOOST_CHECK(v1.compare(v2)==0);
    }

    {
        variant v1(variant::DataTable);
        v1.add_column(variant::DateTime).add_column(variant::Int32).add_column(variant::Double);

        variant::date_time_t time1(variant::date_t(2007, 1, 3), variant::time_t(10, 30, 0));
        variant::date_time_t time2(variant::date_t(2007, 1, 4), variant::time_t(11, 30, 0));

        v1.push_back( boost::make_tuple(time1, 42, 3.141) );
        v1.push_back( boost::make_tuple(time2, 1, 2.718) );

        std::ostringstream oss;
        binary_writer writer(oss);
        writer << v1;

        variant v2;
        std::stringstream iss;
        iss << oss.str();
        binary_reader reader(iss);
        reader >> v2;

        BOOST_CHECK(v1.compare(v2)==0);

        variant d1(variant::DataTable);
        std::ostringstream oss2;
        binary_writer writer2(oss2);
        writer2 << d1;

        variant d2;
        std::stringstream iss2;
        iss2 << oss2.str();
        binary_reader reader2(iss2);
        reader2 >> d2;

        BOOST_CHECK(d1.compare(d2)==0);

        variant e1(variant::DataTable);
        e1.add_column(variant::Int32);
        e1.add_column(variant::String);
        std::ostringstream oss3;
        binary_writer writer3(oss3);
        writer3 << e1;

        variant e2;
        std::stringstream iss3;
        iss3 << oss3.str();
        binary_reader reader3(iss3);
        reader3 >> e2;

        BOOST_CHECK(e1.compare(e2)==0);
    }

    {
        variant v1(variant::DataTable);
        v1.add_column(variant::Int32).add_column(variant::Tuple).add_column(variant::String).add_column(variant::Dictionary);

        int x0 = 42;
        variant x1(variant::Tuple, 2);
            x1.at(0) = variant(3.14);
            x1.at(1) = variant(true);
        std::string x2("Hello");
        variant x3(variant::Dictionary, 2);
            x3.insert("foo", variant(variant::date_t(2007, 1, 3)));
            x3.insert("bar", variant(variant::time_t(10, 30, 0)));

        v1.push_back( make_row(x0, x1, x2, x3) );

        int y0 = 1729;
        variant y1(variant::Tuple, 1);
            y1.at(0) = variant(variant::date_t(2008, 1, 3));
        std::string y2("Bonjour");
        variant y3(variant::Dictionary);

        v1.push_back( make_row(y0, y1, y2, y3) );

        std::ostringstream oss;
        binary_writer writer(oss);
        writer << v1;

        variant v2;
        std::stringstream iss(oss.str());
        binary_reader reader(iss);
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

    /*virtual*/ std::string name() const {
       return "protean::binary_streams::testing_object";
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
    void inflate(const variant& params, int /*version*/)
    {
        m_count = params["count"].as<boost::int32_t>();
        m_id = params["id"].as<std::string>();
    }
private:
    static boost::int32_t  sm_count;
    boost::int32_t  m_count;
    std::string     m_id;
};

boost::int32_t testing_object::sm_count = 0;

BOOST_AUTO_TEST_CASE(test_binary_object)
{
    testing_object arg("test object");
    variant v1(arg);

    std::ostringstream oss;
    binary_writer writer(oss);
    writer << v1;

    variant v2;
    std::stringstream iss1;
    iss1 << oss.str();
    binary_reader reader1(iss1);
    reader1 >> v2;

    BOOST_CHECK(v2.is<object_proxy>());
    BOOST_CHECK(v1.compare(v2)==0);

    variant v3;
    std::stringstream iss2;
    iss2 << oss.str();

    object_factory factory;
    factory.register_object<testing_object>();
    binary_reader reader2(iss2);
    reader2.set_factory(factory);
    reader2 >> v3;

    BOOST_CHECK(!v3.is<object_proxy>());
    BOOST_CHECK(v1.compare(v3)==0);
}

BOOST_AUTO_TEST_CASE(test_binary_simple_array)
{
    typed_array a1(3, variant::String);
    a1[0] = "s1";
    a1[1] = "s2";
    a1[2] = "s3";

    variant v1(a1);

    std::stringstream ss;
    binary_writer writer(ss);
    writer << v1;

    variant v2;
    binary_reader reader(ss);
    reader >> v2;

    BOOST_CHECK(v1.compare(v2)==0);
}

BOOST_AUTO_TEST_CASE(test_binary_complex_array)
{
    typed_array a1(3, variant::Dictionary);
    a1[0] = variant(variant::Dictionary).insert("a", variant(1)).insert("b", variant(2));
    a1[1] = variant(variant::Dictionary).insert("b", variant(3)).insert("c", variant(4));
    a1[2] = variant(variant::Dictionary).insert("d", variant(5)).insert("e", variant(6));

    variant v1(a1);

    std::stringstream ss;
    binary_writer writer(ss);
    writer << v1;

    variant v2;
    binary_reader reader(ss);
    reader >> v2;

    BOOST_CHECK(v1.compare(v2)==0);
}

BOOST_AUTO_TEST_CASE(test_date_time_as_ticks)
{
    variant::date_t date(2010, 1, 1);
    variant::time_t time(boost::posix_time::time_duration(10, 30, 20) + boost::posix_time::millisec(123));
    variant::date_time_t date_time(date, time);

    {
        variant v1(date);

        std::stringstream ss;
        binary_writer writer(ss, binary_mode::DateTimeAsTicks);
        writer << v1;

        variant v2;
        binary_reader reader(ss, binary_mode::DateTimeAsTicks);
        reader >> v2;

        BOOST_CHECK_EQUAL(v1.as<variant::date_t>(), v2.as<variant::date_t>());
    }

    {
        variant v1(time);

        std::stringstream ss;
        binary_writer writer(ss, binary_mode::DateTimeAsTicks);
        writer << v1;

        variant v2;
        binary_reader reader(ss, binary_mode::DateTimeAsTicks);
        reader >> v2;

        BOOST_CHECK_EQUAL(v1.as<variant::time_t>(), v2.as<variant::time_t>());
    }

    {
        variant v1(date_time);

        std::stringstream ss;
        binary_writer writer(ss, binary_mode::DateTimeAsTicks);
        writer << v1;

        variant v2;
        binary_reader reader(ss, binary_mode::DateTimeAsTicks);
        reader >> v2;

        BOOST_CHECK_EQUAL(v1.as<variant::date_time_t>(), v2.as<variant::date_time_t>());
    }

    {
        variant v1(variant::min_date_time());

        std::stringstream ss;
        binary_writer writer(ss, binary_mode::DateTimeAsTicks);
        writer << v1;

        variant v2;
        binary_reader reader(ss, binary_mode::DateTimeAsTicks);
        reader >> v2;

        BOOST_CHECK_EQUAL(v1.as<variant::date_time_t>(), v2.as<variant::date_time_t>());
    }

    {
        variant v1(variant::max_date_time());

        std::stringstream ss;
        binary_writer writer(ss, binary_mode::DateTimeAsTicks);
        writer << v1;

        variant v2;
        binary_reader reader(ss, binary_mode::DateTimeAsTicks);
        reader >> v2;

        BOOST_CHECK_EQUAL(v1.as<variant::date_time_t>(), v2.as<variant::date_time_t>());
    }
}

BOOST_AUTO_TEST_SUITE_END()
