//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef CHECK_VARIANT_FUNCTION
#define CHECK_VARIANT_FUNCTION(TYPE, FUNCTION_CALL)                                                                                                 \
    if (!is<TYPE>())                                                                                                                                \
    {                                                                                                                                               \
        boost::throw_exception(variant_error(std::string("Attempt to call ") + FUNCTION_CALL + " on " + enum_to_string(m_type) + " variant"));      \
    }
#endif // CHECK_VARIANT_FUNCTION

#ifndef BEGIN_VARIANT_CONTEXT
#define BEGIN_VARIANT_CONTEXT                                                                           \
        try                                                                                             \
        {                                                                                               \
	        BOOST_STATIC_ASSERT (true)
#endif // BEGIN_VARIANT_CONTEXT

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
#endif // END_VARIANT_CONTEXT
