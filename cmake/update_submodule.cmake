# Utility function for updating a git submodule.
# Simply calls `git submodule update --init --recursive --rebase -- ${modulePath}`,
# but only if the submodule hasn't already been updated (checks for common repo files).
function(update_submodule modulePath)
	if( NOT (EXISTS ${RYTHE_DIR_ROOT}/${modulePath}/README.md) AND
		NOT (EXISTS ${RYTHE_DIR_ROOT}/${modulePath}/.gitignore) AND 
		NOT (EXISTS ${RYTHE_DIR_ROOT}/${modulePath}/CMakeLists.txt))
		message(STATUS "Downloading ${modulePath}")
		execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive --rebase -- ${modulePath}
						WORKING_DIRECTORY ${RYTHE_DIR_ROOT}
						RESULT_VARIABLE GIT_SUBMOD_RESULT)
		if(NOT GIT_SUBMOD_RESULT EQUAL "0")
			message(FATAL_ERROR "git submodule update failed with ${GIT_SUBMOD_RESULT}")
		endif()
	endif()
endfunction()
