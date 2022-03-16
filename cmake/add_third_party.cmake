# Simple macro that checks if the third party library has previously been added before adding it
macro(rythe_add_third_party)
	set(options CURRENT_HEADER_ONLY LIBRARY_HEADER_ONLY)
	set(oneValueArgs CURRENT LIBRARY PATH INCLUDE FOLDER)
	set(multiValueArgs FOLDER_TARGETS)

	cmake_parse_arguments(RYTHE_ADD_THIRD_PARTY "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

	message(STATUS "Downloading ${RYTHE_ADD_THIRD_PARTY_PATH}")
	
	execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive --rebase -- ${RYTHE_ADD_THIRD_PARTY_PATH}
					WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
					RESULT_VARIABLE GIT_SUBMOD_RESULT)
	if(NOT GIT_SUBMOD_RESULT EQUAL "0")
		message(FATAL_ERROR "git submodule update failed with ${GIT_SUBMOD_RESULT}")
	endif()

    if (NOT TARGET ${RYTHE_ADD_THIRD_PARTY_LIBRARY})
		set(RYTHE_INCLUDE_THIRD_PARTY ${RYTHE_INCLUDE_THIRD_PARTY} ${RYTHE_ADD_THIRD_PARTY_INCLUDE} PARENT_SCOPE)
		
		if (NOT ${RYTHE_ADD_THIRD_PARTY_LIBRARY_HEADER_ONLY})
        	add_subdirectory(${RYTHE_ADD_THIRD_PARTY_PATH})

			foreach(target ${RYTHE_ADD_THIRD_PARTY_FOLDER_TARGETS})
				if (TARGET ${target})
					set_target_properties(${target} PROPERTIES FOLDER ${RYTHE_ADD_THIRD_PARTY_FOLDER})
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
