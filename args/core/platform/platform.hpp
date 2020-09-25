#pragma once
/**
 * @file platform.hpp
 */


/**@def ARGS_CPP17V
 * @brief the version number of c++17 as long
 */
#define ARGS_CPP17V 201703L

#define ARGS_DEBUG_VALUE 1
#define ARGS_RELEASE_VALUE

#if defined(_DEBUG) || defined(DEBUG)
	/**@def ARGS_DEBUG
	 * @brief Defined in debug mode.
	 */
	#define ARGS_DEBUG
    #define ARGS_CONFIGURATION ARGS_DEBUG_VALUE
#else
	/**@def ARGS_RELEASE
	 * @brief Defined in release mode.
	 */
	#define ARGS_RELEASE 
    #define ARGS_CONFIGURATION ARGS_RELEASE_VALUE
#endif

#if (!defined(ARGS_LOW_POWER) && !defined(ARGS_HIGH_PERFORMANCE))
	/**@def ARGS_HIGH_PERFORMANCE
	 * @brief Automatically defined if ARGS_LOW_POWER was not defined. It makes Args ask the hardware's full attention to run as fast as possible.
	 * @note Define ARGS_LOW_POWER to run Args with minimal resources instead.
	 */
	#define ARGS_HIGH_PERFORMANCE
#endif


#if defined(_MSC_VER)
	/**@def ARGS_WINDOWS
	 * @brief Defined when compiling for Windows.
	 */
	#define ARGS_WINDOWS

	#define WIN32_LEAN_AND_MEAN
	#define VC_EXTRALEAN
	#define NOMINMAX
	#include <Windows.h>
#elif defined(__linux__)
	/**@def ARGS_LINUX
	 * @brief Defined when compiling for Linux.
	 */
	#define ARGS_LINUX

	#include <sys/resource.h>
	#include <sched.h>
	#include <errno.h>
#endif

#ifndef __FUNC__
	#define __FUNC__ __func__ 
#endif

#ifndef __FULL_FUNC__
	#if defined(ARGS_WINDOWS)
		#define __FULL_FUNC__ __FUNCSIG__
	#elif defined(__linux__)
		#define __FULL_FUNC__ __PRETTY_FUNCTION__
	#else
		#define __FULL_FUNC__ __func__
	#endif
#endif


#if (defined(ARGS_WINDOWS) && !defined(ARGS_WINDOWS_USE_CDECL)) || defined (DOXY_INCLUDE)
	/**@def ARGS_CCONV
	 * @brief the calling convention exported functions will use in the args engine
	 */
	#define ARGS_CCONV __fastcall
#else
	#define ARGS_CCONV __cdecl
#endif

/**@def NO_MANGLING
 * @brief exports functions with C style names instead of C++ mangled names
 */
#define NO_MANGLING extern "C"

#if defined(ARGS_WINDOWS) || defined(DOXY_INCLUDE)
	#if (defined(ARGS_INTERNAL) && !defined(ARGS_IMPORT)) || defined(DOXY_INCLUDE)

		/**@def ARGS_API
		 * @brief sets the export setting for shared libraries
		 */
		#define ARGS_API __declspec(dllexport)
	#else
		#define ARGS_API __declspec(dllimport)
	#endif
#else
	#define ARGS_API __attribute__((visibility("default")))
#endif

#if defined(ARGS_IMPORT)
#define ARGS_LIBRARY 
#endif

/**@def ARGS_FUNC
 * @brief export setting + calling convention used by the engine
 */
#define ARGS_FUNC ARGS_API ARGS_CCONV

/**@def ARGS_INTERFACE
 * @brief un-mangled function name +  export setting + calling convention used by the engine
 */
#define ARGS_INTERFACE NO_MANGLING ARGS_API ARGS_CCONV 

#if defined(__has_cpp_attribute)|| defined(DOXY_INCLUDE) 
/**@def AHASCPPATTRIB
 * @brief checks if a certain attribute exists in this version of c++
 * @param x attribute you want to test for
 * @return true if attribute exists
 */
#  define AHASCPPATTRIB(x) __has_cpp_attribute(x)
#else
#  define AHASCPPATTRIB(x) 0
#endif



#if __cplusplus >= ARGS_CPP17V || AHASCPPATTRIB(nodiscard) || defined(DOXY_INCLUDE)

/**@def A_NODISCARD
 * @brief Marks a function as "nodiscard" meaning that result must be captured and should not be discarded.
 */
#define A_NODISCARD [[nodiscard]]
#else
#define A_NODISCARD
#endif

#if __cplusplus > ARGS_CPP17V || AHASCPPATTRIB(noreturn) || defined(DOXY_INCLUDE)
/**@def A_NORETURN
 * @brief Marks a function as "noreturn" meaning that the function will never finish, or terminate the application
 */
#define A_NORETURN [[noreturn]]
#else
#define A_NORETURN
#endif

/**@def ARGS_PURE
 * @brief Marks a function as pure virtual.
 */
#define ARGS_PURE =0

/**@def ARGS_IMPURE
 * @brief Marks a function as overridable but default implemented.
 */
#define ARGS_IMPURE {}

/**@def ARGS_IMPURE_RETURN
 * @brief Marks a function as overridable but default implemented with certain default return value.
 * @param x value the function should return.
 */
#define ARGS_IMPURE_RETURN(x) { return (x); }
