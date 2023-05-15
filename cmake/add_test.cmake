macro(rythe_add_catch2)
    set(oneValueArgs PATH)
    cmake_parse_arguments(RYTHE_ADD_CATCH2 ""${oneValueArgs}"" ${ARGN} )

    if(NOT TARGET Catch2)
        if (${RYTHE_FETCH_SUBMODULES})
            message(STATUS "Downloading ${RYTHE_ADD_CATCH2_PATH}")
            
            execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive --rebase -- ${RYTHE_ADD_CATCH2_PATH}
                            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                            RESULT_VARIABLE GIT_SUBMOD_RESULT)
            if(NOT GIT_SUBMOD_RESULT EQUAL "0")
                message(FATAL_ERROR "git submodule update failed with ${GIT_SUBMOD_RESULT}")
            endif()
        endif()

        set(RYTHE_CATCH_DIR ${RYTHE_ADD_CATCH2_PATH} PARENT_SCOPE)
        add_subdirectory(${RYTHE_ADD_CATCH2_PATH})

        if(TARGET Catch2)
            set_target_properties(Catch2 PROPERTIES FOLDER "5 - tests/third_party/catch2")

            get_target_property(target_sources Catch2 SOURCES)
            foreach(src ${target_sources})
                get_filename_component(dir ${src} DIRECTORY)
                source_group("${dir}" FILES "${src}")
            endforeach()
        endif()
    endif()
endmacro()

function(rythe_configure_test target)
    if(NOT TARGET Catch2)
        message(FATAL_ERROR "Catch2 was not found!")
    else()
        # Language
        target_compile_features(${target} PUBLIC cxx_std_20)
        
        # Dependency on Rythe
        target_include_directories(${target} PUBLIC ${RYTHE_INCLUDE_ENGINE})
        target_link_libraries(${target} PUBLIC ${RYTHE_LIBS_ENGINE})

        target_include_directories(${target} PUBLIC ${RYTHE_INCLUDE_THIRD_PARTY})
        target_link_libraries(${target} PUBLIC ${RYTHE_LIBS_THIRD_PARTY})

        target_include_directories(${target} PUBLIC ${RYTHE_CATCH_DIR}/src)
        target_link_libraries(${target} PUBLIC Catch2)

        # Compiler
        target_compile_definitions(${target} PUBLIC ${RYTHE_DEFINITIONS})
        target_compile_options(${target} PUBLIC ${RYTHE_COMPILER_FLAGS})
        
        # Linker
        if (NOT ${RYTHE_LINKER_FLAGS} STREQUAL "")
            target_link_options(${target} PUBLIC ${RYTHE_LINKER_FLAGS})
        endif()
        set_target_properties(${target} PROPERTIES LINKER_LANGUAGE ${RYTHE_LANGUAGE})

        get_target_property(target_sources ${target} SOURCES)
        foreach(src ${target_sources})
            get_filename_component(dir ${src} DIRECTORY)
            file(RELATIVE_PATH rel_path ${PROJECT_SOURCE_DIR} ${dir})
            source_group("${rel_path}" FILES "${src}")
        endforeach()

        list(APPEND RYTHE_ENABLED_APPLICATION_NAMES ${target})
        set(RYTHE_ENABLED_APPLICATION_NAMES ${RYTHE_ENABLED_APPLICATION_NAMES} PARENT_SCOPE)

        message(STATUS "Test: \"${target}\" [Enabled]")
    endif()
endmacro()
