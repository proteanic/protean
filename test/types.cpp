//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>  
#include <boost/cstdint.hpp>

using boost::unit_test::test_suite;    

#include <protean/variant.hpp>
using namespace protean;

BOOST_AUTO_TEST_SUITE( types );

BOOST_AUTO_TEST_CASE( pod_types )
{
    {
        variant v( bool(true) );
        BOOST_CHECK(v.is<variant::Boolean>());
        BOOST_CHECK(v.is<bool>());
        BOOST_CHECK_EQUAL(v.as<bool>(), true);
    }

    {
        variant v( long(0) );
        BOOST_CHECK(v.is<variant::Int32>());
        BOOST_CHECK(v.is<long>());
        BOOST_CHECK_EQUAL(v.as<long>(), 0);
    }


    {
        variant v( 0ul );
        BOOST_CHECK(v.is<variant::UInt32>());
        BOOST_CHECK(v.is<unsigned long>());
        BOOST_CHECK_EQUAL(v.as<unsigned long>(), 0u);
    }

    {
        variant v( int(0) );
        BOOST_CHECK(v.is<variant::Int32>());
        BOOST_CHECK(v.is<int>());
        BOOST_CHECK_EQUAL(v.as<int>(), 0);
    }

    {
        variant v( 0u );
        BOOST_CHECK(v.is<variant::UInt32>());
        BOOST_CHECK(v.is<unsigned int>());
        BOOST_CHECK_EQUAL(v.as<unsigned int>(), 0u);
    }

    {
        variant v( 0ll );
        BOOST_CHECK(v.is<variant::Int64>());
        BOOST_CHECK(v.is<long long>());
        BOOST_CHECK_EQUAL(v.as<long long>(), 0);
    }

    {
        variant v( 0ull );
        BOOST_CHECK(v.is<variant::UInt64>());
        BOOST_CHECK(v.is<unsigned long long>());
        BOOST_CHECK_EQUAL(v.as<unsigned long long>(), 0u);
    }

    {
        variant v( float(0) );
        BOOST_CHECK(v.is<variant::Float>());
        BOOST_CHECK(v.is<float>());
        BOOST_CHECK_EQUAL(v.as<float>(), 0);
    }

    {
        variant v( double(0) );
        BOOST_CHECK(v.is<variant::Double>());
        BOOST_CHECK(v.is<double>());
        BOOST_CHECK_EQUAL(v.as<double>(), 0);
    }
#   ifdef _MSC_VER
    {
        variant v( static_cast<long double>(0) );
        BOOST_CHECK(v.is<variant::Double>());
        BOOST_CHECK(v.is<long double>());
        BOOST_CHECK_EQUAL(v.as<long double>(), 0);
    }
#   endif
}

BOOST_AUTO_TEST_CASE( stl_types )
{
    {
        variant v( size_t(0) );
        BOOST_CHECK(v.is<variant::UInt32>());
        BOOST_CHECK(v.is<size_t>());
        BOOST_CHECK_EQUAL(v.as<size_t>(), 0u);
    }
}

BOOST_AUTO_TEST_CASE( boost_types )
{
    {
        variant v( boost::int32_t(0) );
        BOOST_CHECK(v.is<variant::Int32>());
        BOOST_CHECK(v.is<boost::int32_t>());
        BOOST_CHECK_EQUAL(v.as<boost::int32_t>(), 0);
    }

    {
        variant v( boost::uint32_t(0) );
        BOOST_CHECK(v.is<variant::UInt32>());
        BOOST_CHECK(v.is<boost::uint32_t>());
        BOOST_CHECK_EQUAL(v.as<boost::uint32_t>(), 0u);
    }

    {
        variant v( boost::int64_t(0) );
        BOOST_CHECK(v.is<variant::Int64>());
        BOOST_CHECK(v.is<boost::int64_t>());
        BOOST_CHECK_EQUAL(v.as<boost::int64_t>(), 0);
    }

    {
        variant v( boost::uint64_t(0) );
        BOOST_CHECK(v.is<variant::UInt64>());
        BOOST_CHECK(v.is<boost::uint64_t>());
        BOOST_CHECK_EQUAL(v.as<boost::uint64_t>(), 0u);
    }

}

BOOST_AUTO_TEST_CASE( ms_specific_types )
{
#   ifdef _MSC_VER
    {
        variant v( __int32(0) );
        BOOST_CHECK(v.is<variant::Int32>());
        BOOST_CHECK(v.is<__int32>());
        BOOST_CHECK_EQUAL(v.as<__int32>(), 0);
    }

    {
        variant v( unsigned __int32(0) );
        BOOST_CHECK(v.is<variant::UInt32>());
        BOOST_CHECK(v.is<unsigned __int32>());
        BOOST_CHECK_EQUAL(v.as<unsigned __int32>(), 0);
    }

    {
        variant v( __int64(0) );
        BOOST_CHECK(v.is<variant::Int64>());
        BOOST_CHECK(v.is<__int64>());
        BOOST_CHECK_EQUAL(v.as<__int64>(), 0);
    }

    {
        variant v( unsigned __int64(0) );
        BOOST_CHECK(v.is<variant::UInt64>());
        BOOST_CHECK(v.is<unsigned __int64>());
        BOOST_CHECK_EQUAL(v.as<unsigned __int64>(), 0);
    }
#   endif
}

BOOST_AUTO_TEST_CASE( cast_op )
{
    variant v1("10");
    variant v2(variant::Int32,  v1);
    variant v3(variant::UInt32, v1);
    variant v4(variant::Int64,  v1);
    variant v5(variant::UInt64, v1);
    variant v6(variant::Float,  v1);
    variant v7(variant::Double, v1);

    BOOST_CHECK(v2.is<boost::int32_t>());
    BOOST_CHECK_EQUAL(v2.as<boost::int32_t>(), 10);
    BOOST_CHECK(v3.is<boost::uint32_t>());
    BOOST_CHECK_EQUAL(v3.as<boost::uint32_t>(), 10u);
    BOOST_CHECK(v4.is<boost::int64_t>());
    BOOST_CHECK_EQUAL(v4.as<boost::int64_t>(), 10);
    BOOST_CHECK(v5.is<boost::uint64_t>());
    BOOST_CHECK_EQUAL(v5.as<boost::uint64_t>(), 10u);
    BOOST_CHECK(v6.is<float>());
    BOOST_CHECK_EQUAL(v6.as<float>(), 10);
    BOOST_CHECK(v7.is<double>());
    BOOST_CHECK_EQUAL(v7.as<double>(), 10);
}

BOOST_AUTO_TEST_SUITE_END()
