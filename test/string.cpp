//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>  

using boost::unit_test::test_suite;    

#include <protean/variant.hpp>
using namespace protean;

void test_detail_string()
{
	detail::string vs00;
	BOOST_CHECK_EQUAL(vs00.size(), 0);
	BOOST_CHECK_EQUAL(vs00.onStack(), true);
	BOOST_CHECK_EQUAL(vs00.value(), "");

	detail::string vs01("1");
	BOOST_CHECK_EQUAL(vs01.size(), 1);
	BOOST_CHECK_EQUAL(vs01.onStack(), true);
	BOOST_CHECK_EQUAL(vs01.value(), "1");

	detail::string vs02("1234567");
	BOOST_CHECK_EQUAL(vs02.size(), 7);
	BOOST_CHECK_EQUAL(vs02.onStack(), true);
	BOOST_CHECK_EQUAL(vs02.value(), "1234567");

	detail::string vs03("12345678");
	BOOST_CHECK_EQUAL(vs03.size(), 8);
	BOOST_CHECK_EQUAL(vs03.onStack(), false);
	BOOST_CHECK_EQUAL(vs03.value(), "12345678");

	detail::string vs05(vs01);
	BOOST_CHECK_EQUAL(vs05.size(), 1);
	BOOST_CHECK_EQUAL(vs05.onStack(), true);
	BOOST_CHECK_EQUAL(vs05.value(), "1");

	detail::string vs06(vs03);
	BOOST_CHECK_EQUAL(vs06.size(), 8);
	BOOST_CHECK_EQUAL(vs06.onStack(), false);
	BOOST_CHECK_EQUAL(vs06.value(), "12345678");

	vs05=vs03;
	BOOST_CHECK_EQUAL(vs05.size(), 8);
	BOOST_CHECK_EQUAL(vs05.onStack(), false);
	BOOST_CHECK_EQUAL(vs05.value(), "12345678");

	vs06=vs01;
	BOOST_CHECK_EQUAL(vs06.size(), 1);
	BOOST_CHECK_EQUAL(vs06.onStack(), true);
	BOOST_CHECK_EQUAL(vs06.value(), "1");
}

void test_string()
{
    std::string arg("This is a string");

    variant v1(arg);
    BOOST_CHECK(v1.is<variant::String>());
    BOOST_CHECK(!v1.is<variant::Integer>());
    BOOST_CHECK(!v1.is<variant::Number>());
    BOOST_CHECK(v1.is<variant::Primitive>());
    BOOST_CHECK(v1.is<std::string>());
    BOOST_CHECK_EQUAL(v1.as<std::string>(), arg);
    
    variant v2;
    v2 = arg;
    BOOST_CHECK_EQUAL(v2.as<std::string>(), arg);

    variant v3(v2);
    BOOST_CHECK_EQUAL(v3.as<std::string>(), arg);

    variant v4;
    v4 = v3;
    BOOST_CHECK_EQUAL(v3.as<std::string>(), arg);

    variant v5(variant::String);
    BOOST_CHECK(v5.is<variant::String>());
    BOOST_CHECK(v5.is<std::string>());
    BOOST_CHECK_EQUAL(v5.as<std::string>(), "");
}

test_suite* init_unit_test_suite(int, char* []) 
{
    test_suite* test = BOOST_TEST_SUITE("string type");
    test->add(BOOST_TEST_CASE(&test_detail_string));
    test->add(BOOST_TEST_CASE(&test_string));

    return test;
}
