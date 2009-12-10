#include <iostream>

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>  
#include <protean/variant.hpp>

using boost::unit_test::test_suite;    

using namespace protean;

void trivial ()
{
    protean::variant::date_t date = variant::date_t(2007, 1, 3);
    protean::variant::time_t time = variant::time_t(10, 30, 0);
    protean::variant::date_time_t dateTime = variant::date_time_t(date, time);
    protean::variant v(variant::Dictionary);
    v.insert( "String",   variant( "Element" ) )
     .insert( "Boolean",  variant( true ) )
     .insert( "Integer",  variant( (int)1 ) )
     .insert( "Unsigned", variant( (unsigned)2 ) )
     .insert( "Int32",    variant( (variant::int32_t)0xFFFFFFFFL ) )
     .insert( "UInt32",   variant( (variant::uint32_t)0xFFFFFFFFUL ) )
     .insert( "Int64",    variant( (variant::int64_t)0xFFFFFFFFFFFFFFFFLL ) )
     .insert( "UInt64",   variant( (variant::uint64_t)0xFFFFFFFFFFFFFFFFULL ) )
     .insert( "Float",    variant( (float)3.7 ) )
     .insert( "Double",   variant( (double)3.9 ) )
     .insert( "Date",     variant( date ) )
     .insert( "Time",     variant( time ) )
     .insert( "DateTime", variant( dateTime ) )
     .insert( "None",     variant( variant::None ) );
    BOOST_CHECK( v.is<protean::variant::Dictionary>() );
    BOOST_CHECK_THROW( v["NOT THERE"], variant_error );
    BOOST_CHECK( v["String"].is<std::string>() );
    BOOST_CHECK( v["Boolean"].is<bool>() );
    BOOST_CHECK( v["Integer"].is<int>() );
    BOOST_CHECK( v["Unsigned"].is<unsigned>() );
    BOOST_CHECK( v["Int32"].is<variant::int32_t>() );
    BOOST_CHECK( v["UInt32"].is<variant::uint32_t>() );
    BOOST_CHECK( v["Int64"].is<variant::int64_t>() );
    BOOST_CHECK( v["UInt64"].is<variant::uint64_t>() );
    BOOST_CHECK( v["Float"].is<float>() );
    BOOST_CHECK( v["Double"].is<double>() );
    BOOST_CHECK( v["Date"].is<variant::date_t>() );
    BOOST_CHECK( v["Time"].is<variant::time_t>() );
    BOOST_CHECK( v["DateTime"].is<variant::date_time_t>() );
    BOOST_CHECK( v["None"].is<variant::None>() );
    BOOST_CHECK_EQUAL( v["String"].as<std::string>(), "Element" );
    BOOST_CHECK_EQUAL( v["Boolean"].as<bool>(), true );
    BOOST_CHECK_EQUAL( v["Integer"].as<int>(), 1 );
    BOOST_CHECK_EQUAL( v["Unsigned"].as<unsigned>(), (unsigned)2 );
    BOOST_CHECK_EQUAL( v["Int32"].as<variant::int32_t>(), (long)0xFFFFFFFFL );
    BOOST_CHECK_EQUAL( v["UInt32"].as<variant::uint32_t>(), 0xFFFFFFFFUL );
    BOOST_CHECK_EQUAL( v["Int64"].as<variant::int64_t>(), (long long)0xFFFFFFFFFFFFFFFFLL );
    BOOST_CHECK_EQUAL( v["UInt64"].as<variant::uint64_t>(), 0xFFFFFFFFFFFFFFFFULL );
    BOOST_CHECK_EQUAL( v["Float"].as<float>(), (float)3.7 );
    BOOST_CHECK_EQUAL( v["Double"].as<double>(), 3.9 );
    BOOST_CHECK_EQUAL( v["Date"].as<variant::date_t>(), date );
    BOOST_CHECK_EQUAL( v["Time"].as<variant::time_t>(), time );
    BOOST_CHECK_EQUAL( v["DateTime"].as<variant::date_time_t>(), dateTime );
}

test_suite* init_unit_test_suite(int, char* []) 
{
    test_suite* test = BOOST_TEST_SUITE("trivial tests");
    test->add(BOOST_TEST_CASE(&trivial));
    return test;
}
