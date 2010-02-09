//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CONFIG_AUTO_LINK_HPP
#define PROTEAN_CONFIG_AUTO_LINK_HPP

#ifndef PROTEAN_VERSION_HPP
#  include <protean/version.hpp>
#endif

#ifndef PROTEAN_LIB_NAME
#   error "Macro PROTEAN_LIB_NAME not set (internal error)"
#endif

#if defined(_MSC_VER)

#if (_MSC_VER <= 1400)
#   define PROTEAN_LIB_TOOLSET "vc80"
#elif (_MSC_VER == 1500)
#   define PROTEAN_LIB_TOOLSET "vc90"
#elif (_MSC_VER >= 1600)
#   define PROTEAN_LIB_TOOLSET "vc100"
#endif

#if defined(_MT) || defined(__MT__)
#   define PROTEAN_LIB_THREAD_OPT "-mt"
#else
#  define PROTEAN_LIB_THREAD_OPT
#endif

#if defined(_DEBUG)
#   define PROTEAN_LIB_RT_OPT "-gd"
#else
#   define PROTEAN_LIB_RT_OPT
#endif

#if defined(PROTEAN_DYN_LINK)
#   define PROTEAN_LIB_PREFIX
#else
#   define PROTEAN_LIB_PREFIX "lib"
#endif

//
// now include the lib
//
#define PROTEAN_STRINGIZE(X) PROTEAN_DO_STRINGIZE(X)
#define PROTEAN_DO_STRINGIZE(X) #X

#pragma comment(lib, PROTEAN_LIB_PREFIX PROTEAN_STRINGIZE(PROTEAN_LIB_NAME) "-" PROTEAN_LIB_TOOLSET PROTEAN_LIB_THREAD_OPT PROTEAN_LIB_RT_OPT "-" PROTEAN_LIB_VERSION ".lib")

#endif

#ifdef PROTEAN_LIB_PREFIX
#   undef PROTEAN_LIB_PREFIX
#endif
#if defined(PROTEAN_LIB_NAME)
#   undef PROTEAN_LIB_NAME
#endif
#if defined(PROTEAN_LIB_TOOLSET)
#   undef PROTEAN_LIB_TOOLSET
#endif
#if defined(PROTEAN_LIB_THREAD_OPT)
#   undef PROTEAN_LIB_THREAD_OPT
#endif
#if defined(PROTEAN_LIB_RT_OPT)
#   undef PROTEAN_LIB_RT_OPT
#endif

#endif // PROTEAN_CONFIG_AUTO_LINK_HPP
