//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef PROTEAN_VERSION_HPP
#define PROTEAN_VERSION_HPP

#define PROTEAN_MAJOR_VERSION 1
#define PROTEAN_MINOR_VERSION 4
#define PROTEAN_PATCH_VERSION 0

//
//  PROTEAN_VERSION % 100 is the patch level
//  PROTEAN_VERSION / 100 % 1000 is the minor version
//  PROTEAN_VERSION / 100000 is the major version

#define PROTEAN_VERSION 100400

//
//  PROTEAN_LIB_VERSION must be defined to be the same as PROTEAN_VERSION
//  but as a *string* in the form "x_y[_z]" where x is the major version
//  number, y is the minor version number, and z is the patch level if not 0.

#define PROTEAN_LIB_VERSION "1_04"

#endif // PROTEAN_VERSION_HPP
