# Simple macro that checks if the third party library has previously been added before adding it
macro(rythe_add_third_party)
	set(options CURRENT_HEADER_ONLY LIBRARY_HEADER_ONLY)
	set(oneValueArgs CURRENT LIBRARY PATH INCLUDE FOLDER)
	set(multiValueArgs FOLDER_TARGETS)

	cmake_parse_arguments(RYTHE_ADD_THIRD_PARTY "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

	if (${RYTHE_FETCH_SUBMODULES})
		message(STATUS "Downloading ${RYTHE_ADD_THIRD_PARTY_PATH}")
		
		execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive --rebase -- ${RYTHE_ADD_THIRD_PARTY_PATH}
						WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
						RESULT_VARIABLE GIT_SUBMOD_RESULT)
		if(NOT GIT_SUBMOD_RESULT EQUAL "0")
			message(FATAL_ERROR "git submodule update failed with ${GIT_SUBMOD_RESULT}")
		endif()
	endif()

    if (NOT TARGET ${RYTHE_ADD_THIRD_PARTY_LIBRARY})
		set(RYTHE_INCLUDE_THIRD_PARTY ${RYTHE_INCLUDE_THIRD_PARTY} ${RYTHE_ADD_THIRD_PARTY_INCLUDE})
		set(RYTHE_INCLUDE_THIRD_PARTY ${RYTHE_INCLUDE_THIRD_PARTY} PARENT_SCOPE)
		
		# Bit of an exception: Core adds its dependencies to all other modules
		if (${RYTHE_ADD_THIRD_PARTY_CURRENT} STREQUAL "core")
			set(RYTHE_INCLUDE_ALL ${RYTHE_INCLUDE_ALL} PARENT_SCOPE)
			set(RYTHE_INCLUDE_ALL ${RYTHE_INCLUDE_ALL} ${RYTHE_ADD_THIRD_PARTY_INCLUDE})

			set(RYTHE_INCLUDE_ENGINE ${RYTHE_INCLUDE_ENGINE} PARENT_SCOPE)
			set(RYTHE_INCLUDE_ENGINE ${RYTHE_INCLUDE_ENGINE} ${RYTHE_ADD_THIRD_PARTY_INCLUDE})
		endif()

		if (NOT ${RYTHE_ADD_THIRD_PARTY_LIBRARY_HEADER_ONLY})
        	add_subdirectory(${RYTHE_ADD_THIRD_PARTY_PATH})

			foreach(target ${RYTHE_ADD_THIRD_PARTY_FOLDER_TARGETS})
				if (TARGET ${target})
					string(REGEX REPLACE "^third_party" "4 - third party" filter ${RYTHE_ADD_THIRD_PARTY_FOLDER})
					set_target_properties(${target} PROPERTIES FOLDER ${filter})

					get_target_property(target_sources ${target} SOURCES)
					foreach(src ${target_sources})
						get_filename_component(dir ${src} DIRECTORY)
						source_group("${dir}" FILES "${src}")
					endforeach()
				endif()
			endforeach()
			
			set(RYTHE_LIBS_THIRD_PARTY ${RYTHE_LIBS_THIRD_PARTY} ${RYTHE_ADD_THIRD_PARTY_LIBRARY})
			set(RYTHE_LIBS_THIRD_PARTY ${RYTHE_LIBS_THIRD_PARTY} PARENT_SCOPE)
		endif()
    endif()

	if (NOT ${RYTHE_ADD_THIRD_PARTY_CURRENT_HEADER_ONLY})
		target_include_directories(${RYTHE_ADD_THIRD_PARTY_CURRENT} PUBLIC ${RYTHE_ADD_THIRD_PARTY_INCLUDE})
	endif()
endmacro()
