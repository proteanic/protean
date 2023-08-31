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
        BOOST_CHECK(v.is<sizeof(long)==4?variant::Int32:variant::Int64>());
        BOOST_CHECK(v.is<long>());
        BOOST_CHECK_EQUAL(v.as<long>(), 0);
    }


    {
        variant v( 0ul );
        BOOST_CHECK(v.is<sizeof(long)==4?variant::UInt32:variant::UInt64>());
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
        BOOST_CHECK(v.is<sizeof(size_t)==4?variant::UInt32:variant::UInt64>());
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
        BOOST_CHECK_EQUAL(v.as<unsigned __int32>(), 0u);
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
        BOOST_CHECK_EQUAL(v.as<unsigned __int64>(), 0u);
    }

	{
        variant i(3);
        variant j(4);
		i<j;
		BOOST_CHECK(i<j);
	}

#   endif
}

BOOST_AUTO_TEST_CASE(lexical_cast)
{
    BOOST_CHECK_EQUAL(variant(variant::Boolean, "true").as<bool>(),             true);
    BOOST_CHECK_EQUAL(variant(variant::Boolean, "false").as<bool>(),            false);
    BOOST_CHECK_EQUAL(variant(variant::Boolean, "1").as<bool>(),                true);
    BOOST_CHECK_EQUAL(variant(variant::Boolean, "0").as<bool>(),                false);
    BOOST_CHECK_EQUAL(variant(variant::Int32,   "10").as<boost::int32_t>(),     10);
    BOOST_CHECK_EQUAL(variant(variant::UInt32,  "10").as<boost::uint32_t>(),    10u);
    BOOST_CHECK_EQUAL(variant(variant::Int64,   "10").as<boost::int64_t>(),     10);
    BOOST_CHECK_EQUAL(variant(variant::UInt64,  "10").as<boost::uint64_t>(),    10u);
    BOOST_CHECK_EQUAL(variant(variant::Float,   "10").as<float>(),              10.);
    BOOST_CHECK_EQUAL(variant(variant::Double,  "10").as<double>(),             10.);
	BOOST_CHECK_EQUAL(variant(variant::DateTime, "2014-03-21T15:50:31").as<variant::date_time_t>(), variant::date_time_t(variant::date_t(2014, 3, 21), variant::time_t(15, 50, 31)));

    BOOST_CHECK_THROW(variant(variant::Boolean, "###"),   variant_error);
    BOOST_CHECK_THROW(variant(variant::Boolean, "###"),   variant_error);
    BOOST_CHECK_THROW(variant(variant::Boolean, "###"),   variant_error);
    BOOST_CHECK_THROW(variant(variant::Boolean, "###"),   variant_error);
    BOOST_CHECK_THROW(variant(variant::Int32,   "###"),   variant_error);
    BOOST_CHECK_THROW(variant(variant::UInt32,  "###"),   variant_error);
    BOOST_CHECK_THROW(variant(variant::Int64,   "###"),   variant_error);
    BOOST_CHECK_THROW(variant(variant::UInt64,  "###"),   variant_error);
    BOOST_CHECK_THROW(variant(variant::Float,   "###"),   variant_error);
    BOOST_CHECK_THROW(variant(variant::Double,  "###"),   variant_error);
	BOOST_CHECK_THROW(variant(variant::DateTime, "###"),  variant_error);
}

BOOST_AUTO_TEST_CASE(numerical_cast)
{
    BOOST_CHECK_EQUAL(variant(boost::int32_t(1)).numerical_cast<double>(), 1.);
    BOOST_CHECK_EQUAL(variant(boost::int32_t(1)).numerical_cast<bool>(), true);
    BOOST_CHECK_EQUAL(variant(boost::int32_t(0)).numerical_cast<double>(), 0.);
    BOOST_CHECK_EQUAL(variant(boost::int32_t(0)).numerical_cast<bool>(), false);
    BOOST_CHECK_EQUAL(variant(1.).numerical_cast<boost::int32_t>(), 1);
    BOOST_CHECK_EQUAL(variant(1.).numerical_cast<bool>(), true);
    BOOST_CHECK_EQUAL(variant(0.).numerical_cast<boost::int32_t>(), 0);
    BOOST_CHECK_EQUAL(variant(0.).numerical_cast<bool>(), false);

    BOOST_CHECK_THROW(variant("1.").numerical_cast<double>(), variant_error);
}

BOOST_AUTO_TEST_SUITE_END()
