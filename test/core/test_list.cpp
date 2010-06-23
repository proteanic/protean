//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>  

using boost::unit_test::test_suite;    

#include <protean/variant.hpp>
using namespace protean;

BOOST_AUTO_TEST_SUITE(list_suite);

BOOST_AUTO_TEST_CASE(test_list_basic)
{
    variant v1(variant::List);
    BOOST_CHECK(v1.is<variant::List>());
    BOOST_CHECK(v1.is<variant::Sequence>());
    BOOST_CHECK(!v1.is<variant::Mapping>());
    BOOST_CHECK(!v1.is<variant::Primitive>());
   
    BOOST_CHECK(v1.empty());
    BOOST_CHECK_EQUAL(v1.size(), 0u);

    boost::int32_t arg1(0xFFFFFFFF);
    std::string arg2("This is a string");

    v1.push_back(variant(arg1));
    v1.push_back(variant(arg2));

    BOOST_CHECK(!v1.empty());
    BOOST_REQUIRE(v1.size()==2);

    BOOST_CHECK(v1[0].is<boost::int32_t>());
    BOOST_CHECK_EQUAL(v1[0].as<boost::int32_t>(), arg1);
    BOOST_CHECK(v1[1].is<std::string>());
    BOOST_CHECK_EQUAL(v1[1].as<std::string>(), arg2);

    // test equality
    variant v2(variant::List);
    BOOST_CHECK(v1!=v2);

    v2.push_back(variant(arg1));
    v2.push_back(variant(arg2));
    BOOST_CHECK(v1==v2);

    variant v3(v2);
    BOOST_CHECK(v3.is<variant::List>());
    BOOST_CHECK(v3==v2);

    variant v4;
    v4 = v3;
    BOOST_CHECK(v4.is<variant::List>());
    BOOST_CHECK(v4==v3);

    // test hashing
    BOOST_CHECK_EQUAL(v1.hash(), 0x29280912e5afd22eUL);
}

BOOST_AUTO_TEST_CASE(test_list_iterator)
{
    variant v1(variant::List);

    BOOST_CHECK(v1.begin()==v1.end());

    boost::int32_t arg1(0xFFFFFFFF);
    std::string arg2("This is a string");

    v1.push_back(variant(arg1));
    v1.push_back(variant(arg2));

    BOOST_CHECK(v1.begin()!=v1.end());

    variant::iterator begin(v1.begin());
    variant::iterator end(v1.end());

    BOOST_CHECK_THROW(begin.key(), variant_error);
    BOOST_CHECK_THROW(begin.time(), variant_error);

    BOOST_CHECK(begin->is<boost::int32_t>());
    BOOST_CHECK_EQUAL(begin->as<boost::int32_t>(), arg1);
    BOOST_CHECK_EQUAL(begin.value().as<boost::int32_t>(), arg1);

    *begin = arg2;
    BOOST_CHECK(begin->is<std::string>());
    BOOST_CHECK_EQUAL(begin->as<std::string>(), arg2);

    ++begin;
    BOOST_CHECK(begin->is<std::string>());
    BOOST_CHECK_EQUAL(begin->as<std::string>(), arg2);

    ++begin;
    BOOST_CHECK(begin==end);
}

BOOST_AUTO_TEST_SUITE_END()
