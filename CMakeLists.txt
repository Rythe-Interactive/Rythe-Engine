# Root CMake build file for Rythe-Engine
# Sets up the build environment for Rythe engine, editor, applications, and samples.
# Refer to the readme or docs for information on its usage.

cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(rythe LANGUAGES CXX)

# Configuration
option(RYTHE_FORCE_ENABLE_ALL_MODULES "Forcefully enable all modules (overrides module toggles)" OFF)
option(RYTHE_BUILD_OPTION_ASAN "Enable the address sanitizer where possible" OFF)
option(RYTHE_BUILD_OPTION_WERROR "Treat warnings as errors in all of Rythe's libraries and applications" OFF)
option(RYTHE_BUILD_APPLICATIONS "Build applications such as samples and sandbox apps" ON)
option(RYTHE_FETCH_SUBMODULES "Enable fetching required submodules. Recommended when configuring modules/dependencies. Turn off if you're just adjusting files within the project." ON)

# Folder structure
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY ON)
set(CMAKE_FOLDER CMakePredefinedTargets)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

# Convenience directory variables, usable in subdirs
set(RYTHE_DIR_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(RYTHE_DIR_RYTHE ${CMAKE_CURRENT_SOURCE_DIR}/rythe)
set(RYTHE_DIR_ENGINE ${CMAKE_CURRENT_SOURCE_DIR}/rythe/engine)
set(RYTHE_DIR_EDITOR ${CMAKE_CURRENT_SOURCE_DIR}/rythe/editor)

set(RYTHE_DIR_OUTPUT_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(RYTHE_DIR_OUTPUT_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include)

# C++ files/lang
set(RYTHE_LANGUAGE CXX)
set(RYTHE_FILE_TYPES *.h *.hpp *.c *.cpp *.inl)

# Include dirs
set(RYTHE_INCLUDE_ALL "")
set(RYTHE_INCLUDE_ENGINE "")
set(RYTHE_INCLUDE_EDITOR "")
set(RYTHE_INCLUDE_THIRD_PARTY "")

# Libs
set(RYTHE_LIBS_ALL "")
set(RYTHE_LIBS_ENGINE "")
set(RYTHE_LIBS_EDITOR "")
set(RYTHE_LIBS_THIRD_PARTY "")

# Definitions
set(RYTHE_DEFINITIONS "")
list(APPEND RYTHE_DEFINITIONS "RYTHE_ENGINE")

# Compiler flags
set(RYTHE_COMPILER_FLAGS "")
set(RYTHE_LINKER_FLAGS "")

if (NOT MSVC)
	set(RYTHE_COMPILER_FLAGS -Wall -Wextra -Wpedantic
							-Wno-c++98-compat -Wno-c++98-compat-pedantic 
							-Wno-c++0x-compat 
							-Wno-c++11-compat -Wno-c++11-compat-pedantic
							-Wno-c++14-compat -Wno-c++14-compat-pedantic
							${RYTHE_COMPILER_FLAGS})

    if (${RYTHE_BUILD_OPTION_WERROR})
        set(RYTHE_COMPILER_FLAGS -Werror ${RYTHE_COMPILER_FLAGS})
    endif()

	if (${RYTHE_BUILD_OPTION_ASAN})
		set(RYTHE_COMPILER_FLAGS -fsanitize=address ${RYTHE_COMPILER_FLAGS})
		set(RYTHE_LINKER_FLAGS -fsanitize=address ${RYTHE_LINKER_FLAGS})
	endif()
else()
	set(RYTHE_COMPILER_FLAGS /MP /W4 ${RYTHE_COMPILER_FLAGS})

    if (${RYTHE_BUILD_OPTION_WERROR})
        set(RYTHE_COMPILER_FLAGS /WX ${RYTHE_COMPILER_FLAGS})
    endif()

	if (${RYTHE_BUILD_OPTION_ASAN}})
		set(RYTHE_COMPILER_FLAGS /fsanitize=address ${RYTHE_COMPILER_FLAGS})
	endif()
endif()

# Force library type to static
set(RYTHE_LIBRARY_TYPE STATIC)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
mark_as_advanced(FORCE BUILD_SHARED_LIBS) # Hide

# Third party libraries often like to enable docs/examples
# we practically never want this from our dependencies
set(BUILD_DOC OFF CACHE BOOL "" FORCE)
set(BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_SANDBOX OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

# Utility files
include("${CMAKE_MODULE_PATH}/update_submodule.cmake")
include("${CMAKE_MODULE_PATH}/copy_module_output.cmake")
include("${CMAKE_MODULE_PATH}/configure_application.cmake")
include("${CMAKE_MODULE_PATH}/configure_module.cmake")
include("${CMAKE_MODULE_PATH}/add_third_party.cmake")
include("${CMAKE_MODULE_PATH}/find_sources.cmake")

# Get Git
find_package(Git REQUIRED)

execute_process(COMMAND ${GIT_EXECUTABLE} config --global core.longpaths true
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	RESULT_VARIABLE GIT_SUBMOD_RESULT)

if(NOT GIT_SUBMOD_RESULT EQUAL "0")
	message(FATAL_ERROR "git submodule update failed with ${GIT_SUBMOD_RESULT}")
endif()

# Add Rythe
set(RYTHE_ENABLED_MODULE_NAMES "")
set(RYTHE_ENABLED_MODULE_PATHS "")
add_subdirectory(${RYTHE_DIR_RYTHE})

# The engine is ready - add applications
if (${RYTHE_BUILD_APPLICATIONS})
	add_subdirectory(${RYTHE_DIR_ROOT}/applications)

	foreach(path ${RYTHE_ENABLED_MODULE_PATHS})
		add_subdirectory(${RYTHE_DIR_ROOT}/${path}/applications)
	endforeach()
endif()
