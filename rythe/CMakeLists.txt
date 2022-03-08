# Rythe engine/editor cmake file
# Gathers modules from git submodules
    # Rythe engine modules are detected and become toggleable
    # Enabled modules are added to the build system
    # Matching editor modules are added if their engine module is enabled

# Clear previously created output directory for header and lib files
execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${RYTHE_DIR_OUTPUT_INCLUDES}/)
execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${RYTHE_DIR_OUTPUT_LIBS}/)

# Parse submodules
include("${CMAKE_MODULE_PATH}/parse_submodules.cmake")

if (NOT ${SUBMODULE_COUNT} EQUAL 0)
    math(EXPR ITER_END "${SUBMODULE_COUNT} - 1")

    foreach(INDEX RANGE 0 ${ITER_END})
        # Get submodule info
        list(GET SUBMODULE_NAMES ${INDEX} SUBMODULE_NAME)
        list(GET SUBMODULE_PATHS ${INDEX} SUBMODULE_PATH)
        list(GET SUBMODULE_DIRS ${INDEX} SUBMODULE_DIR)
        list(GET SUBMODULE_HASHES ${INDEX} SUBMODULE_HASH)

        # Only add modules if they are in the rythe/engine folder
        # Editor modules are added separately based on the engine modules
        string(SUBSTRING ${SUBMODULE_PATH} 0 12 SUBMODULE_NAME_PREFIX)
        if (NOT SUBMODULE_NAME_PREFIX STREQUAL "rythe/engine")
            continue()
        endif()

        # Convert the name to uppercase to follow CMake all caps naming
        string(TOUPPER ${SUBMODULE_NAME} SUBMODULE_VARIABLE_NAME)

        # Core isn't optional so no need for an option variable
        if (NOT SUBMODULE_NAME STREQUAL "core")
            option(RYTHE_MODULE_${SUBMODULE_VARIABLE_NAME} "Adds module ${SUBMODULE_NAME} from the build if enabled" OFF)
        endif()

        # Add if either A) this is core, B) the module is enabled, or C) all modules are forcefully enabled
        if ((SUBMODULE_NAME STREQUAL "core") OR
            (${RYTHE_MODULE_${SUBMODULE_VARIABLE_NAME}}) OR 
            (${RYTHE_FORCE_ENABLE_ALL_MODULES}))

            message(STATUS "Module: ${SUBMODULE_NAME} [Enabled]")
            # MODULE_INFO variables are updated by the module
            # see cmake/configure_module.cmake
            set(MODULE_INFO_HAS_HEADERS OFF)
            set(MODULE_INFO_HAS_SOURCES OFF)
            set(MODULE_INFO_SOURCES "")

            rythe_update_submodule(${SUBMODULE_PATH})
            add_subdirectory(${RYTHE_DIR_ROOT}/${SUBMODULE_PATH}/src/${SUBMODULE_NAME})
            set_target_properties(${SUBMODULE_NAME} PROPERTIES FOLDER ${SUBMODULE_DIR})

            # No need to add as an include path if no headers are present
            if (${MODULE_INFO_HAS_HEADERS})
                list(APPEND RYTHE_INCLUDE_ALL "${RYTHE_DIR_ROOT}/${SUBMODULE_PATH}/src")
                list(APPEND RYTHE_INCLUDE_ENGINE "${RYTHE_DIR_ROOT}/${SUBMODULE_PATH}/src")
            endif()

            # We have to check explicitly because otherwise
            # targets will attempt to link with ${SUBMODULE_NAME}, which won't
            # exist if the target has no cpp or c files
            if (${MODULE_INFO_HAS_SOURCES})
                list(APPEND RYTHE_LIBS_ALL ${SUBMODULE_NAME})
                list(APPEND RYTHE_LIBS_ENGINE ${SUBMODULE_NAME})
            endif()
            
            list(APPEND RYTHE_DEFINITIONS "RYTHE_${SUBMODULE_VARIABLE_NAME}")

            list(APPEND RYTHE_ENABLED_MODULE_NAMES ${SUBMODULE_NAME})
            list(APPEND RYTHE_ENABLED_MODULE_PATHS ${SUBMODULE_PATH})

            # Try to find a matching editor
            list(FIND SUBMODULE_PATHS "rythe/editor/modules/${SUBMODULE_NAME}" SUBMODULE_EDITOR_INDEX)
            if (NOT ${SUBMODULE_EDITOR_INDEX} EQUAL -1)
                # Get submodule info
                list(GET SUBMODULE_NAMES ${SUBMODULE_EDITOR_INDEX} SUBMODULE_EDITOR_NAME)
                list(GET SUBMODULE_PATHS ${SUBMODULE_EDITOR_INDEX} SUBMODULE_EDITOR_PATH)
                list(GET SUBMODULE_DIRS ${SUBMODULE_EDITOR_INDEX} SUBMODULE_EDITOR_DIR)
                list(GET SUBMODULE_HASHES ${SUBMODULE_EDITOR_INDEX} SUBMODULE_EDITOR_HASH)

                message(STATUS "Editor: ${SUBMODULE_EDITOR_NAME} found!")
                rythe_update_submodule("${SUBMODULE_EDITOR_PATH}")
                add_subdirectory("${SUBMODULE_EDITOR_PATH}/src/${SUBMODULE_NAME}-editor")
                set_target_properties("${SUBMODULE_NAME}-editor" PROPERTIES FOLDER ${SUBMODULE_EDITOR_DIR})

                list(APPEND RYTHE_LIBS_ALL "${SUBMODULE_NAME}-editor")
                list(APPEND RYTHE_LIBS_EDITOR "${SUBMODULE_NAME}-editor")
                list(APPEND RYTHE_INCLUDE_ALL "${RYTHE_DIR_ROOT}/${SUBMODULE_EDITOR_PATH}/src")
                list(APPEND RYTHE_INCLUDE_EDITOR "${RYTHE_DIR_ROOT}/${SUBMODULE_EDITOR_PATH}/src")
                list(APPEND RYTHE_DEFINITIONS "RYTHE_${SUBMODULE_VARIABLE_NAME}_EDITOR")
            endif()
        else()
            message(STATUS "Module: ${SUBMODULE_NAME} [Disabled]")
        endif()
    endforeach()
endif()

# Use RYTHE_LIBS to set a few shared features
# such as include dirs, definitions, and compiler/linker flags.
foreach(library ${RYTHE_LIBS_ENGINE})
    set_target_properties(${library} PROPERTIES LINKER_LANGUAGE ${RYTHE_LANGUAGE})

	target_include_directories(${library} PUBLIC ${RYTHE_INCLUDE_ENGINE})
	target_compile_definitions(${library} PUBLIC ${RYTHE_DEFINITIONS})
	target_compile_options(${library} PUBLIC ${RYTHE_COMPILER_FLAGS})
    target_compile_features(${library} PUBLIC cxx_std_17)
	target_link_options(${library} PUBLIC ${RYTHE_LINKER_FLAGS})
endforeach()

foreach(library ${RYTHE_LIBS_EDITOR})
    set_target_properties(${library} PROPERTIES LINKER_LANGUAGE ${RYTHE_LANGUAGE})

    target_include_directories(${library} PUBLIC ${RYTHE_INCLUDE_ALL})
	target_compile_definitions(${library} PUBLIC ${RYTHE_DEFINITIONS})
    target_compile_options(${library} PUBLIC ${RYTHE_COMPILER_FLAGS})
    target_compile_features(${library} PUBLIC cxx_std_17)
    target_link_options(${library} PUBLIC ${RYTHE_LINKER_FLAGS})
endforeach()

# Re-set all the modified RYTHE variables but with PARENT_SCOPE enabled
# so that it properly updates in the root scope.
set(RYTHE_LIBS_ALL ${RYTHE_LIBS_ALL} PARENT_SCOPE)
set(RYTHE_LIBS_ENGINE ${RYTHE_LIBS_ENGINE} PARENT_SCOPE)
set(RYTHE_INCLUDE_ALL ${RYTHE_INCLUDE_ALL} PARENT_SCOPE)
set(RYTHE_INCLUDE_ENGINE ${RYTHE_INCLUDE_ENGINE} PARENT_SCOPE)
set(RYTHE_DEFINITIONS ${RYTHE_DEFINITIONS} PARENT_SCOPE)

set(RYTHE_ENABLED_MODULE_NAMES ${RYTHE_ENABLED_MODULE_NAMES} PARENT_SCOPE)
set(RYTHE_ENABLED_MODULE_PATHS ${RYTHE_ENABLED_MODULE_PATHS} PARENT_SCOPE)
