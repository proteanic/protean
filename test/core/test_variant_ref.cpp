#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>  

using boost::unit_test::test_suite;    

#include <protean/variant.hpp>
#include <protean/variant_ref.hpp>
using namespace protean;

BOOST_AUTO_TEST_SUITE(variant_ref_suite);

BOOST_AUTO_TEST_CASE(test_variant_ref)
{
    variant v1("test string");

    variant_ref ref(v1);

    variant v2(ref);
    variant v3;
    v3 = ref;

    BOOST_CHECK(v1.is<std::string>());
    BOOST_CHECK_EQUAL(v1.as<std::string>(), "test string");

    BOOST_CHECK(v2.is<std::string>());
    BOOST_CHECK_EQUAL(v2.as<std::string>(), "test string");

    BOOST_CHECK(v3.is<std::string>());
    BOOST_CHECK_EQUAL(v3.as<std::string>(), "test string");

    variant v4("other string");
    ref = v4;

    BOOST_CHECK(v1.is<std::string>());
    BOOST_CHECK_EQUAL(v1.as<std::string>(), "other string");

    variant v5(1.);
    BOOST_CHECK_THROW(ref = v5, variant_error);
}

BOOST_AUTO_TEST_CASE(test_variant_cref)
{
    variant v1("test string");

    variant_cref cref(v1);

    variant v2(cref);
    variant v3;
    v3 = cref;

    BOOST_CHECK(v1.is<std::string>());
    BOOST_CHECK_EQUAL(v1.as<std::string>(), "test string");

    BOOST_CHECK(v2.is<std::string>());
    BOOST_CHECK_EQUAL(v2.as<std::string>(), "test string");

    BOOST_CHECK(v3.is<std::string>());
    BOOST_CHECK_EQUAL(v3.as<std::string>(), "test string");
}

BOOST_AUTO_TEST_SUITE_END()
