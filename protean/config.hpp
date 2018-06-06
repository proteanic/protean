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

#if !defined(PROTEAN_SOURCE) && !defined(PROTEAN_NO_AUTOLINK)
#   define PROTEAN_LIB_NAME "protean"
#   include <protean/config/auto_link.hpp>
#endif

#if !defined(XERCES_BINARY)
#if defined(_DEBUG)
#    if defined XERCES_STATIC_LIBRARY
#       define XERCES_BINARY "xerces-c_static_3D"
#    else
#       define XERCES_BINARY "xerces-c_3D"
#    endif
#else
#    if defined XERCES_STATIC_LIBRARY
#       define XERCES_BINARY "xerces-c_static_3"
#    else
#       define XERCES_BINARY "xerces-c_3"
#    endif
#endif
#endif

#if defined (_MSC_VER)
#   define DEPRECATED(x) __declspec(deprecated(x))
#else
#   define DEPRECATED(x)
#endif

#endif // PROTEAN_CONFIG_HPP
