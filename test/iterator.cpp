#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>  

using boost::unit_test::test_suite;    

#include <protean/variant.hpp>
#include <iterator>
#include <algorithm>

using namespace protean;

BOOST_AUTO_TEST_SUITE(iterator_suite);

BOOST_AUTO_TEST_CASE(const_iterator)
{
    variant v1(variant::List);
    variant::iterator i(v1.begin());
    variant::const_iterator ci(v1.end());

    BOOST_CHECK(i==i);
    BOOST_CHECK(ci==ci);
    BOOST_CHECK(i==ci);
    BOOST_CHECK(ci==i);
}

BOOST_AUTO_TEST_CASE(iterator_assign)
{
    variant v1(variant::List);
	v1.push_back(variant(1.0));

    variant::iterator i(v1.begin());
	variant::iterator i1(i);
	variant::iterator i2;
	i2 = i;

    BOOST_CHECK(i==i);
    BOOST_CHECK(i1==i);
    BOOST_CHECK(i2==i);

    BOOST_CHECK(i==v1.begin());
    BOOST_CHECK(i1==v1.begin());
    BOOST_CHECK(i2==v1.begin());

    BOOST_CHECK(i!=v1.end());
    BOOST_CHECK(i1!=v1.end());
    BOOST_CHECK(i2!=v1.end());

	++i;

    BOOST_CHECK(i==i);
    BOOST_CHECK(i1!=i);
    BOOST_CHECK(i2!=i);

    BOOST_CHECK(i==v1.end());
    BOOST_CHECK(i1==v1.begin());
    BOOST_CHECK(i2==v1.begin());
}

BOOST_AUTO_TEST_CASE(traits_value)
{
    typedef std::iterator_traits<variant::iterator>::value_type value;
    
    variant v(static_cast<boost::int32_t>(17));
    value val = v;
    BOOST_CHECK_EQUAL (val.as<boost::int32_t>(),17);
}

BOOST_AUTO_TEST_CASE(traits_ptr_ref)
{
    typedef std::iterator_traits<variant::iterator>::value_type value;
    typedef std::iterator_traits<variant::iterator>::pointer vptr;
    typedef std::iterator_traits<variant::iterator>::reference vref;
    
    variant v(static_cast<boost::int32_t>(1111));
    vref ref (v);
    vptr ptr = &v;
    BOOST_CHECK_EQUAL (v.as<boost::int32_t>(),1111);
    ref = variant(static_cast<boost::int32_t>(7));
    BOOST_CHECK_EQUAL (v.as<boost::int32_t>(),7);
    *ptr = variant(static_cast<boost::int32_t>(42));
    BOOST_CHECK_EQUAL (v.as<boost::int32_t>(),42);
}

BOOST_AUTO_TEST_CASE(traits_diff)
{
    typedef std::iterator_traits<variant::iterator>::difference_type diff;

    variant l(variant::List);
    diff zero = 0;
    BOOST_CHECK (l.end () == zero + l.begin ());
}

#if 0
BOOST_AUTO_TEST_CASE(algorithms)
{
   variant l(variant::List), l1=1;
   variant v(static_cast<boost::int32_t>(1111));
   l.push_back (v);
   l.push_back (v);
   std::copy(l.begin(), l.end(), std::back_inserter(l1));
   BOOST_CHECK_EQUAL(true,std::equal (l.begin(), l.end(), l1.begin()));
   BOOST_CHECK_EQUAL(l1.size(),2);
}
#endif

BOOST_AUTO_TEST_SUITE_END()
