//  (C) Copyright Johan Ditmar, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHECK_VARIANT_FUNCTION
#define CHECK_VARIANT_FUNCTION(TYPE, FUNCTION)                                                                                                      \
    if (!is<TYPE>())                                                                                                                                \
    {                                                                                                                                               \
        boost::throw_exception(variant_error(std::string("Attempt to call ") + #FUNCTION + "() on " + enum_to_string(m_type) + " variant"));        \
    }
#endif

#ifndef CHECK_VARIANT
#define CHECK_VARIANT(TYPE)                                                                                                                         \
    if (!is<TYPE>())                                                                                                                                \
    {                                                                                                                                               \
        boost::throw_exception(variant_error(std::string("Attempt to call ") + __FUNCTION__ + "() on " + enum_to_string(m_type) + " variant"));     \
    }
#endif

#ifndef BEGIN_VARIANT_CONTEXT
#define BEGIN_VARIANT_CONTEXT                                                                           \
        try                                                                                             \
        {                                                                                               \
	        BOOST_STATIC_ASSERT (true)
#endif

#ifndef END_VARIANT_CONTEXT
#define END_VARIANT_CONTEXT                                                                             \
        }                                                                                               \
        catch(std::exception& e)                                                                        \
        {                                                                                               \
            boost::throw_exception(variant_error(std::string(e.what()) + "\n" + this->str()));          \
        }                                                                                               \
        catch(...)                                                                                      \
        {                                                                                               \
            boost::throw_exception(variant_error(std::string("Unknown exception\n") + this->str()));    \
        }                                                                                               \
        BOOST_STATIC_ASSERT (true)
#endif
