#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>  

using boost::unit_test::test_suite;    

#include <protean/variant.hpp>
using namespace protean;

BOOST_AUTO_TEST_SUITE(number_suite);

BOOST_AUTO_TEST_CASE(test_int32)
{
    boost::int32_t arg(0xFFFFFFFF);

    variant v1(arg);
    BOOST_CHECK(v1.is<variant::Int32>());
    BOOST_CHECK(v1.is<variant::Integer>());
    BOOST_CHECK(v1.is<variant::Number>());
    BOOST_CHECK(v1.is<variant::Primitive>());
    BOOST_CHECK_EQUAL(v1.as<boost::int32_t>(), arg);
    
    variant v2;
    v2 = arg;
    BOOST_CHECK_EQUAL(v2.as<boost::int32_t>(), arg);

    variant v3(v2);
    BOOST_CHECK_EQUAL(v3.as<boost::int32_t>(), arg);

    variant v4;
    v4 = v3;
    BOOST_CHECK_EQUAL(v3.as<boost::int32_t>(), arg);

    variant v5(variant::Int32);
    BOOST_CHECK(v5.is<variant::Int32>());
    BOOST_CHECK_EQUAL(v5.as<boost::int32_t>(), 0);

    BOOST_CHECK_THROW(v1.as<boost::uint32_t>(), variant_error);

    BOOST_CHECK_EQUAL(v1.hash(), 0xfaf89879581633a9UL);
}

BOOST_AUTO_TEST_CASE(test_uint32)
{
    boost::uint32_t arg(0xFFFFFFFF);

    variant v1(arg);
    BOOST_CHECK(v1.is<variant::UInt32>());
    BOOST_CHECK(v1.is<variant::Integer>());
    BOOST_CHECK(v1.is<variant::Number>());
    BOOST_CHECK(v1.is<variant::Primitive>());
    BOOST_CHECK_EQUAL(v1.as<boost::uint32_t>(), arg);
    
    variant v2;
    v2 = arg;
    BOOST_CHECK_EQUAL(v2.as<boost::uint32_t>(), arg);

    variant v3(v2);
    BOOST_CHECK_EQUAL(v3.as<boost::uint32_t>(), arg);

    variant v4;
    v4 = v3;
    BOOST_CHECK_EQUAL(v3.as<boost::uint32_t>(), arg);

    variant v5(variant::UInt32);
    BOOST_CHECK(v5.is<variant::UInt32>());
    BOOST_CHECK_EQUAL(v5.as<boost::uint32_t>(), 0u);

    BOOST_CHECK_THROW(v1.as<boost::int32_t>(), variant_error);

    BOOST_CHECK_EQUAL(v1.hash(), 0x82fb587a448e8822UL);
}

BOOST_AUTO_TEST_CASE(test_int64)
{
    boost::int64_t arg(0xFFFFFFFFFFFFFFFFull);

    variant v1(arg);
    BOOST_CHECK(v1.is<variant::Int64>());
    BOOST_CHECK(v1.is<variant::Integer>());
    BOOST_CHECK(v1.is<variant::Number>());
    BOOST_CHECK(v1.is<variant::Primitive>());
    BOOST_CHECK_EQUAL(v1.as<boost::int64_t>(), arg);
    
    variant v2;
    v2 = arg;
    BOOST_CHECK_EQUAL(v2.as<boost::int64_t>(), arg);

    variant v3(v2);
    BOOST_CHECK_EQUAL(v3.as<boost::int64_t>(), arg);

    variant v4;
    v4 = v3;
    BOOST_CHECK_EQUAL(v3.as<boost::int64_t>(), arg);

    variant v5(variant::Int64);
    BOOST_CHECK(v5.is<variant::Int64>());
    BOOST_CHECK_EQUAL(v5.as<boost::int64_t>(), 0);

    BOOST_CHECK_THROW(v1.as<boost::uint64_t>(), variant_error);

    BOOST_CHECK_EQUAL(v1.hash(), 0x28f2daae23d14c0UL);
}

BOOST_AUTO_TEST_CASE(test_uInt64)
{
    boost::uint64_t arg(0xFFFFFFFFFFFFFFFFull);

    variant v1(arg);
    BOOST_CHECK(v1.is<variant::UInt64>());
    BOOST_CHECK(v1.is<variant::Integer>());
    BOOST_CHECK(v1.is<variant::Number>());
    BOOST_CHECK(v1.is<variant::Primitive>());
    BOOST_CHECK_EQUAL(v1.as<boost::uint64_t>(), arg);
    
    variant v2;
    v2 = arg;
    BOOST_CHECK_EQUAL(v2.as<boost::uint64_t>(), arg);

    variant v3(v2);
    BOOST_CHECK_EQUAL(v3.as<boost::uint64_t>(), arg);

    variant v4;
    v4 = v3;
    BOOST_CHECK_EQUAL(v3.as<boost::uint64_t>(), arg);

    variant v5(variant::UInt64);
    BOOST_CHECK(v5.is<variant::UInt64>());
    BOOST_CHECK_EQUAL(v5.as<boost::uint64_t>(), 0u);

    BOOST_CHECK_THROW(v1.as<boost::int64_t>(), variant_error);

    BOOST_CHECK_EQUAL(v1.hash(), 0x421b797986c2e5a5UL);
}

BOOST_AUTO_TEST_CASE(test_float)
{
    float arg(std::numeric_limits<float>::max());

    variant v1(arg);
    BOOST_CHECK(v1.is<variant::Float>());
    BOOST_CHECK(!v1.is<variant::Integer>());
    BOOST_CHECK(v1.is<variant::Number>());
    BOOST_CHECK(v1.is<variant::Primitive>());
    BOOST_CHECK_EQUAL(v1.as<float>(), arg);
    
    variant v2;
    v2 = arg;
    BOOST_CHECK_EQUAL(v2.as<float>(), arg);

    variant v3(v2);
    BOOST_CHECK_EQUAL(v3.as<float>(), arg);

    variant v4;
    v4 = v3;
    BOOST_CHECK_EQUAL(v3.as<float>(), arg);

    variant v5(variant::Float);
    BOOST_CHECK(v5.is<variant::Float>());
    BOOST_CHECK_EQUAL(v5.as<float>(), 0.);

    BOOST_CHECK_EQUAL(v1.hash(), 0x5340a61f9264fa3bUL);
}

BOOST_AUTO_TEST_CASE(test_double)
{
    double arg(std::numeric_limits<double>::max());

    variant v1(arg);
    BOOST_CHECK(v1.is<variant::Double>());
    BOOST_CHECK(!v1.is<variant::Integer>());
    BOOST_CHECK(v1.is<variant::Number>());
    BOOST_CHECK(v1.is<variant::Primitive>());
    BOOST_CHECK_EQUAL(v1.as<double>(), arg);
    
    variant v2;
    v2 = arg;
    BOOST_CHECK_EQUAL(v2.as<double>(), arg);

    variant v3(v2);
    BOOST_CHECK_EQUAL(v3.as<double>(), arg);

    variant v4;
    v4 = v3;
    BOOST_CHECK_EQUAL(v3.as<double>(), arg);

    variant v5(variant::Double);
    BOOST_CHECK(v5.is<variant::Double>());
    BOOST_CHECK_EQUAL(v5.as<double>(), 0.);

    BOOST_CHECK_EQUAL(v1.hash(), 0x1feb970e905230ecUL);
}

BOOST_AUTO_TEST_CASE(test_boolean)
{
    bool arg(true);

    variant v1(arg);
    BOOST_CHECK(v1.is<variant::Boolean>());
    BOOST_CHECK(v1.is<variant::Integer>());
    BOOST_CHECK(v1.is<variant::Number>());
    BOOST_CHECK(v1.is<variant::Primitive>());
    BOOST_CHECK_EQUAL(v1.as<bool>(), arg);
    
    variant v2;
    v2 = arg;
    BOOST_CHECK_EQUAL(v2.as<bool>(), arg);

    variant v3(v2);
    BOOST_CHECK_EQUAL(v3.as<bool>(), arg);

    variant v4;
    v4 = v3;
    BOOST_CHECK_EQUAL(v3.as<bool>(), arg);

    variant v5(variant::Boolean);
    BOOST_CHECK(v5.is<variant::Boolean>());
    BOOST_CHECK_EQUAL(v5.as<bool>(), false);

    BOOST_CHECK_EQUAL(v1.hash(), 0x3db28790f6c316a6UL);
}

BOOST_AUTO_TEST_SUITE_END()
