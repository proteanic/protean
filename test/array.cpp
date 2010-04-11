//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>  

using boost::unit_test::test_suite;    

#include <protean/variant.hpp>
using namespace protean;

BOOST_AUTO_TEST_SUITE(array_suite);

BOOST_AUTO_TEST_CASE(test_array_basic)
{
    BOOST_CHECK_THROW(variant v(variant::Array), variant_error);

    typed_array a1(5, variant::String);
    BOOST_REQUIRE_EQUAL(a1.size(), 5);
    BOOST_REQUIRE_EQUAL(a1.type(), variant::String);

    for (size_t i=0; i<5; ++i)
    {
        BOOST_CHECK(a1[i].as<std::string>().empty());
    }

    a1[0] = "s1";
    a1[1] = "s2";
    a1[2] = "s3";
    a1[3] = "s4";
    a1[4] = "s5";

    BOOST_CHECK_EQUAL(a1[0].as<std::string>(), "s1");
    BOOST_CHECK_EQUAL(a1[1].as<std::string>(), "s2");
    BOOST_CHECK_EQUAL(a1[2].as<std::string>(), "s3");
    BOOST_CHECK_EQUAL(a1[3].as<std::string>(), "s4");
    BOOST_CHECK_EQUAL(a1[4].as<std::string>(), "s5");

    typed_array a2(5, variant("s6"));
    BOOST_REQUIRE_EQUAL(a1.size(), 5);
    BOOST_REQUIRE_EQUAL(a1.type(), variant::String);

    for (size_t i=0; i<5; ++i)
    {
        BOOST_CHECK_EQUAL(a2[i].as<std::string>(), "s6");
    }

    // check comparison
    BOOST_CHECK_EQUAL(a1.compare(a1), 0);
    BOOST_CHECK_EQUAL(a2.compare(a2), 0);
    BOOST_CHECK_EQUAL(a1.compare(a2), -1);
    BOOST_CHECK_EQUAL(a2.compare(a1), 1);

    BOOST_CHECK_THROW(a1[0] = 0, variant_error);
}

BOOST_AUTO_TEST_CASE(test_array_iterators)
{
    typed_array a1(3, variant::String);
    BOOST_REQUIRE_EQUAL(a1.size(), 3);
    BOOST_REQUIRE_EQUAL(a1.type(), variant::String);

    typed_array::iterator it1(a1.begin());
    *it1 = "s1";
    ++it1;
    *it1 = "s2";
    ++it1;
    *it1 = "s3";
    ++it1;

    BOOST_CHECK(it1==a1.end());

    BOOST_CHECK_EQUAL(a1[0].as<std::string>(), "s1");
    BOOST_CHECK_EQUAL(a1[1].as<std::string>(), "s2");
    BOOST_CHECK_EQUAL(a1[2].as<std::string>(), "s3");

    typed_array::const_iterator it2(a1.begin());
    BOOST_CHECK_EQUAL(it2->as<std::string>(), "s1");
    ++it2;
    BOOST_CHECK_EQUAL(it2->as<std::string>(), "s2");
    ++it2;
    BOOST_CHECK_EQUAL(it2->as<std::string>(), "s3");
    ++it2;

    BOOST_CHECK(it2==a1.end());
}

BOOST_AUTO_TEST_CASE(test_array_variant)
{
    typed_array a1(5, variant::String);
    a1[0] = "s1";
    a1[1] = "s2";
    a1[2] = "s3";
    a1[3] = "s4";
    a1[4] = "s5";

    typed_array a2(5, variant("s6"));

    variant v1(a1);
    variant v2(a2);

    BOOST_CHECK(v1.is<variant::Array>());
    BOOST_CHECK(v2.is<variant::Array>());

    const typed_array& a3(v1.as<typed_array>());
    BOOST_CHECK_EQUAL(a3[0].as<std::string>(), "s1");
    BOOST_CHECK_EQUAL(a3[1].as<std::string>(), "s2");
    BOOST_CHECK_EQUAL(a3[2].as<std::string>(), "s3");
    BOOST_CHECK_EQUAL(a3[3].as<std::string>(), "s4");
    BOOST_CHECK_EQUAL(a3[4].as<std::string>(), "s5");

    const typed_array& a4(v2.as<typed_array>());
    for (size_t i=0; i<5; ++i)
    {
        BOOST_CHECK_EQUAL(a4[i].as<std::string>(), "s6");
    }

    BOOST_CHECK_EQUAL(v1.compare(v1), 0);
    BOOST_CHECK_EQUAL(v2.compare(v2), 0);
    BOOST_CHECK_EQUAL(v1.compare(v2), -1);
    BOOST_CHECK_EQUAL(v2.compare(v1), 1);
}

BOOST_AUTO_TEST_SUITE_END()
