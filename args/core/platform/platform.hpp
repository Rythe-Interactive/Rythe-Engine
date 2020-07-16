#pragma once
/**
 * @file platform.hpp
 */


/**@def ARGS_CPP17V
 * @brief the version number of c++17 as long
 */
#define ARGS_CPP17V 201703L

#if defined(_MSC_VER)
 #define ARGS_WINDOWS
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
	#if defined(ARGS_INTERNAL) || defined(DOXY_INCLUDE)

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
#define ARGS_IMPURE_RETURN(x) { return x; }
