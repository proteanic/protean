//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CONFIG_HPP
#define PROTEAN_CONFIG_HPP

#include <protean/version.hpp>

#if defined(PROTEAN_DYN_LINK)
#if defined(PROTEAN_SOURCE)
#   define PROTEAN_DECL __declspec(dllexport)
#else
#   define PROTEAN_DECL __declspec(dllimport)
#endif
#endif

#ifndef PROTEAN_DECL
#   define PROTEAN_DECL
#endif

#if !defined(PROTEAN_SOURCE)
#   define PROTEAN_LIB_NAME protean
//#   include <protean/config/auto_link.hpp>
#endif

#endif // PROTEAN_CONFIG_HPP
