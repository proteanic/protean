//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>  

using boost::unit_test::test_suite;    

#include <protean/variant.hpp>
using namespace protean;

void primitive()
{
    variant v1(1.0);
    BOOST_CHECK(v1.is<variant::Double>());
    BOOST_CHECK_EQUAL(v1.as<double>(), 1.0);
}

void sequence()
{
}

void mapping()
{
}

void timeseries()
{
}

void buffer()
{
}

void exception()
{
}

test_suite* init_unit_test_suite(int, char* []) 
{
    test_suite* test = BOOST_TEST_SUITE("basic types");
    test->add(BOOST_TEST_CASE(&primitive));
    test->add(BOOST_TEST_CASE(&sequence));
    test->add(BOOST_TEST_CASE(&mapping));
    test->add(BOOST_TEST_CASE(&timeseries));
    test->add(BOOST_TEST_CASE(&buffer));
    test->add(BOOST_TEST_CASE(&exception));
    return test;
}
