#pragma once

/**@file args_library.hpp
 * @brief This file should be included after all the args headers if the project you're building is a library.
 *        ARGS_IMPORT needs to be defined before all args headers.
 */

// Allow ARGS_API to work for dll's that depend on core.
#if defined(ARGS_IMPORT) && !defined(DOXY_INCLUDE)
    #undef ARGS_API
    #if defined(ARGS_WINDOWS)
        #if defined(ARGS_INTERNAL)
            #define ARGS_API //__declspec(dllexport)
        #else
            #define ARGS_API //__declspec(dllimport)
        #endif
    #else
        #define ARGS_API __attribute__((visibility("default")))
    #endif

    #undef ARGS_IMPORT
#endif
