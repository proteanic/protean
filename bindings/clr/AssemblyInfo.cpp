//  (C) Copyright Johan Ditmar, Karel Hruda, Paul O'Neill & Luke Stedman 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include <protean/version.hpp>

#include <boost/preprocessor.hpp>

using namespace System::Reflection;
using namespace System::Runtime::CompilerServices;
using namespace System::Runtime::InteropServices;

//
// General Information about an assembly is controlled through the following
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
//
[assembly:AssemblyTitleAttribute("Protean CLR Library")];
[assembly:AssemblyDescriptionAttribute("")];
[assembly:AssemblyConfigurationAttribute("")];
[assembly:AssemblyCompanyAttribute("")];
[assembly:AssemblyProductAttribute("Protean CLR Library")];
[assembly:AssemblyCopyrightAttribute("Copyright (c) 2010, Johan Ditmar, Karel Hruda, Paul O'Neill and Luke Stedman")];
[assembly:AssemblyTrademarkAttribute("")];
[assembly:AssemblyCultureAttribute("")];

//
// Version information for an assembly consists of the following four values:
//
//      Major Version
//      Minor Version
//      Build Number
//      Revision
//
// You can specify all the value or you can default the Revision and Build Numbers
// by using the '*' as shown below:

#define PROTEAN_ASSEMBLY_VERSION \
	BOOST_PP_STRINGIZE(PROTEAN_MAJOR_VERSION) ## "." ## BOOST_PP_STRINGIZE(PROTEAN_MINOR_VERSION) ## BOOST_PP_STRINGIZE(PROTEAN_PATCH_VERSION) ## ".*"

#define PROTEAN_ASSEMBLY_FILE_VERSION \
	BOOST_PP_STRINGIZE(PROTEAN_MAJOR_VERSION) ## "." ## BOOST_PP_STRINGIZE(PROTEAN_MINOR_VERSION) ## BOOST_PP_STRINGIZE(PROTEAN_PATCH_VERSION) ## ".0"

[assembly:AssemblyVersionAttribute(PROTEAN_ASSEMBLY_VERSION)];
[assembly:AssemblyFileVersionAttribute(PROTEAN_ASSEMBLY_FILE_VERSION)];

[assembly:ComVisible(false)];

