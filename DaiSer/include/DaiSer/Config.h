#pragma once

#ifdef _WIN32
#	define DAISER_SYSTEM_WIN
#	define NOMINMAX
#endif

#ifndef DAISER_STATIC

#ifdef DAISER_SYSTEM_WIN
#	define DAISER_API_EXPORT __declspec(dllexport)
#	define DAISER_API_IMPORT __declspec(dllimport)
#	ifdef _MSC_VER
#		pragma warning(disable : 4251) // disable 4251 warning
#	endif
#endif

#else

// no import/export for static build
#define DAISER_API_EXPORT
#define DAISER_API_IMPORT

#endif

#ifndef DAISER_API
#	ifdef DLL_EXPORT
#		define DAISER_API DAISER_API_EXPORT
#	else
#		define DAISER_API DAISER_API_IMPORT
#	endif
#endif

#ifdef __cplusplus
#	if defined(_MSVC_LANG) && _MSVC_LANG > __cplusplus
#		define CXX_VERSION _MSVC_LANG
#	else
#		define CXX_VERSION __cplusplus
#	endif 
#else
#	error No C++ support? Huh.
#endif

#ifndef C17_SUPPORT
#	if CXX_VERSION > 201402L
#		define C17_SUPPORT 1
#	else
#		define C17_SUPPORT 0
#	endif
#endif

#ifndef C20_SUPPORT
#	if CXX_VERSION && CXX_VERSION > 201703L
#		define C20_SUPPORT 1
#	else
#		define C20_SUPPORT 0
#	endif
#endif

#ifndef C23_SUPPORT
#	if C20_SUPPORT && CXX_VERSION > 202002L
#		define C23_SUPPORT 1
#	else
#		define C23_SUPPORT 0
#	endif
#endif 

#undef CXX_VERSION

#if C20_SUPPORT && !C17_SUPPORT
#	error C20_SUPPORT must imply C17_SUPPORT.
#endif

#if C23_SUPPORT && !C20_SUPPORT
#	error C23_SUPPORT must imply C20_SUPPORT.
#endif

#ifdef _MSC_VER
#   define DAISER_PRETTY_FUNCTION __FUNCSIG__
#elif defined(__clang__) || defined(__GNUC__)
#   define DAISER_PRETTY_FUNCTION __PRETTY_FUNCTION__
#endif

#if C20_SUPPORT
#	define NOADDRESS [[no_unique_address]]
#else 
#	define NOADDRESS
#endif 

#if C17_SUPPORT
#	define NODISC [[nodiscard]]
#	define UNSD [[maybe_unused]]
#else
#	define NODISC
#	define UNSD
#endif

#define DEPREC [[deprecated]] // C14 support is assumed

#ifndef FULL_NAMESPACE
namespace DaiSer {}
namespace ds = DaiSer;
namespace DS = DaiSer;
#endif 
