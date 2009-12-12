//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>  

using boost::unit_test::test_suite;    

#include <protean/variant.hpp>
using namespace protean;

class testing_object : public object
{
public:
    testing_object() :
        m_count(++sm_count)
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

public:
    object_handle clone() const
    {
        return object_handle(new testing_object(*this));
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
    void inflate(const variant& params, int version)
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

void test_object()
{
    testing_object arg1;
    arg1.set_id("test1");
    variant v1(arg1);

    BOOST_CHECK(v1.is<variant::Object>());
    BOOST_CHECK(v1.is<object>());
    BOOST_CHECK(v1.is<testing_object>());
    BOOST_CHECK(!v1.is<object_proxy>());

    testing_object& arg2(v1.as<testing_object>());
    BOOST_CHECK_EQUAL(arg2.name(), "testing_object");
    BOOST_CHECK_EQUAL(arg2.id(), "test1");

    BOOST_CHECK_EQUAL(v1.as<testing_object>().count(), v1.as<testing_object>().count());

    variant v2(v1);
    BOOST_CHECK(v1.as<testing_object>().count()!=v2.as<testing_object>().count());

    v2.as<testing_object>().set_id("test2");
    BOOST_CHECK_EQUAL(v1.as<testing_object>().id(), "test1");
    BOOST_CHECK_EQUAL(v2.as<testing_object>().id(), "test2");

    const variant v3(v1);
    const variant v4(v1);
    BOOST_CHECK(v3.as<testing_object>().count()==v4.as<testing_object>().count());

    testing_object arg4;
    BOOST_CHECK(arg1.compare(arg4)!=0);
    
    variant params;
    arg1.deflate(params);
    arg4.inflate(params, arg4.version());

    BOOST_CHECK(arg1.compare(arg4)==0);
}

void test_object_proxy()
{
	object_proxy proxy("testing_object");

    testing_object obj1;
    obj1.set_id("test1");

	variant params;
	obj1.deflate(params);
	proxy.inflate(params, 1);

	variant v(proxy);
	BOOST_CHECK(v.is<object_proxy>());
	BOOST_CHECK(v.is<testing_object>());

	testing_object obj2(v.as<testing_object>());

	// Proxy should have been converted to concrete object by as().
	BOOST_CHECK(!v.is<object_proxy>());
	BOOST_CHECK(v.is<testing_object>());
}

test_suite* init_unit_test_suite(int, char* []) 
{
    test_suite* test = BOOST_TEST_SUITE("object type");
    test->add(BOOST_TEST_CASE(&test_object));
    test->add(BOOST_TEST_CASE(&test_object_proxy));

    return test;
}
