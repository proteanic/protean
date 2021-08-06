#ifndef PROTEAN_CONFIG_AUTO_LINK_HPP
#define PROTEAN_CONFIG_AUTO_LINK_HPP

#ifndef PROTEAN_VERSION_HPP
#include <protean/version.hpp>
#endif

#ifndef PROTEAN_LIB_NAME
	#error "Macro PROTEAN_LIB_NAME not set (internal error)"
#endif

#ifdef _MSC_VER

	#if _MSC_VER >= 1920
		#define PROTEAN_LIB_TOOLSET "vc142"
	#elif _MSC_VER >= 1910
		#define PROTEAN_LIB_TOOLSET "vc141"
	#elif _MSC_VER >= 1900
		#define PROTEAN_LIB_TOOLSET "vc140"
	#elif _MSC_VER >= 1800
		#define PROTEAN_LIB_TOOLSET "vc120"
	#endif

	#if defined(_MT) || defined(__MT__)
		#define PROTEAN_LIB_THREAD_OPT "-mt"
	#else
		#define PROTEAN_LIB_THREAD_OPT
	#endif

	#ifdef _DEBUG
		#define PROTEAN_LIB_RT_OPT "-gd"
	#else
		#define PROTEAN_LIB_RT_OPT
	#endif

	#ifdef PROTEAN_DYN_LINK
		#define PROTEAN_LIB_PREFIX
	#else
		#define PROTEAN_LIB_PREFIX "lib"
	#endif

	#pragma comment(lib, PROTEAN_LIB_PREFIX PROTEAN_LIB_NAME "-" PROTEAN_LIB_TOOLSET PROTEAN_LIB_THREAD_OPT PROTEAN_LIB_RT_OPT "-" PROTEAN_LIB_VERSION ".lib")

#endif

#ifdef PROTEAN_LIB_PREFIX
	#undef PROTEAN_LIB_PREFIX
#endif
#ifdef PROTEAN_LIB_NAME
	#undef PROTEAN_LIB_NAME
#endif
#ifdef PROTEAN_LIB_TOOLSET
	#undef PROTEAN_LIB_TOOLSET
#endif
#ifdef PROTEAN_LIB_THREAD_OPT
	#undef PROTEAN_LIB_THREAD_OPT
#endif
#ifdef PROTEAN_LIB_RT_OPT
	#undef PROTEAN_LIB_RT_OPT
#endif

#endif // PROTEAN_CONFIG_AUTO_LINK_HPP
