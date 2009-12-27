//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_CONFIG_HPP
#define PROTEAN_CONFIG_HPP

#if defined(PROTEAN_BUILD_DLL) //Build dll
#    define PROTEAN_DLLEXPORT __declspec(dllexport)
#elif defined(PROTEAN_USE_DLL) //Use dll
#    define PROTEAN_DLLEXPORT __declspec(dllimport)
#else
#    define PROTEAN_DLLEXPORT
#endif

#endif // PROTEAN_CONFIG_HPP
