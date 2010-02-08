//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>  

using boost::unit_test::test_suite;    

#include <protean/variant.hpp>
#include <protean/object_factory.hpp>
using namespace protean;

BOOST_AUTO_TEST_SUITE(object_suite);

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

    /*virtual*/ std::string name() const {
       return "protean::object::testing_object";
    }

public:
    handle<object> clone() const
    {
        return handle<object>(new testing_object(*this));
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

BOOST_AUTO_TEST_CASE(test_object)
{
    testing_object arg1;
    arg1.set_id("test1");
    variant v1(arg1);

    BOOST_CHECK(v1.is<variant::Object>());
    BOOST_CHECK(v1.is<object>());
    BOOST_CHECK(v1.is<testing_object>());
    BOOST_CHECK(!v1.is<object_proxy>());

    const testing_object& arg2(v1.as<testing_object>());
    BOOST_CHECK_EQUAL(arg2.name(), arg1.name());
    BOOST_CHECK_EQUAL(arg2.id(), "test1");

    BOOST_CHECK_EQUAL(v1.as<testing_object>().count(), v1.as<testing_object>().count());

    const variant v2(v1);
    const variant v3(v1);
    BOOST_CHECK(v2.as<testing_object>().count()==v3.as<testing_object>().count());

    testing_object arg4;
    BOOST_CHECK(arg1.compare(arg4)!=0);
    
    variant params;
    arg1.deflate(params);
    arg4.inflate(params, arg4.version());

    BOOST_CHECK(arg1.compare(arg4)==0);

    {
        variant params(variant::Dictionary);
        params.insert("count", variant(boost::int32_t(10)))
            .insert("id", variant("my id"));

        variant v = make_object<testing_object>(params);

        const testing_object& obj(v.as<testing_object>());
        BOOST_CHECK_EQUAL(obj.id(),     "my id");
        BOOST_CHECK_EQUAL(obj.count(),  10);
    }
}

BOOST_AUTO_TEST_CASE(test_object_proxy)
{
	object_proxy proxy("protean::object::testing_object");

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

class a : public testing_object 
{
public:

    std::string name () const { return "protean::a"; }

    handle<object> clone() const
    {
        return handle<object>(new a(*this));
    }
}; 

class b : public testing_object
{
public:

    std::string name () const { return "protean::b"; }

    handle<object> clone() const
    {
        return handle<object>(new b(*this));
    }
}; 

class c : public b
{
public:

    std::string name () const { return "protean::c"; }

    handle<object> clone() const
    {
	   return handle<object>(new c(*this));
    }
}; 

 
BOOST_AUTO_TEST_CASE(test_is_as) 
{ 
    a ai; 
    b bi; 
    c ci; 
    variant va(ai), vb(bi), vc(ci); 
  
    BOOST_CHECK (!va.is<b>());
    BOOST_CHECK (!vb.is<c>());

    BOOST_CHECK (va.is<a> ()); 
    BOOST_CHECK (vb.is<b> ()); 
    BOOST_CHECK (vc.is<c> ()); 
    BOOST_CHECK (vc.is<b> ()); 
    BOOST_CHECK (vc.is<testing_object> ()); 
    BOOST_CHECK (vc.is<object> ()); 
 
    // was ai cloned? 
    BOOST_CHECK (&va.as<a>()!=&ai); 

    // don't slice 
    const object & o0 = vc.as<c> (); 
    BOOST_CHECK_EQUAL (typeid(o0).name (), typeid(c).name ()); 

    const object & o1 = vc.as<object> (); 
    BOOST_CHECK_EQUAL (typeid(o1).name (), typeid(c).name ()); 
}

BOOST_AUTO_TEST_CASE(test_object_factory)
{
    object_factory f;
    BOOST_CHECK_THROW (f.register_object<object_proxy>(), variant_error);
    f.register_object<a> ();
    std::string name = a().name ();

    handle<object> x(f.create_instance(name)), y(f.create_instance(name));
    BOOST_CHECK (x.is<a>());
    BOOST_CHECK (&*x!=&*y);
}

BOOST_AUTO_TEST_SUITE_END()
