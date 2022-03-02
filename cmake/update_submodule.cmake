# Utility function for updating a git submodule.
# Simply calls `git submodule update --init --recursive --rebase -- ${modulePath}`,
# but only if the submodule hasn't already been updated (checks for common repo files).
function(update_submodule modulePath)
	if( NOT (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${modulePath}/README.md) AND
		NOT (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${modulePath}/.gitignore) AND 
		NOT (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${modulePath}/CMakeLists.txt))
		message(STATUS "Downloading ${modulePath}")
		execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive --rebase -- ${modulePath}
						WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
						RESULT_VARIABLE GIT_SUBMOD_RESULT)
		if(NOT GIT_SUBMOD_RESULT EQUAL "0")
			message(FATAL_ERROR "git submodule update failed with ${GIT_SUBMOD_RESULT}")
		endif()
	endif()
endfunction()
