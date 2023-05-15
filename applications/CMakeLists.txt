# Overarching CMake script for applications.
# The Rythe root CMakeLists adds this folder as a submodule,
# this script doesn't have to do much other than add every known application in.

if(${RYTHE_BUILD_APPLICATIONS})
    add_subdirectory(sandbox)
    add_subdirectory(rsl_test)
endif()

set(RYTHE_ENABLED_APPLICATION_NAMES ${RYTHE_ENABLED_APPLICATION_NAMES} PARENT_SCOPE)
