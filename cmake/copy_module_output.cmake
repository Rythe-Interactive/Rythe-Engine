# Utility file that defines the copy_module_output() function.
# copy_module_output adds commands to targets that copy library and header files,
# to the designated rythe include/ and lib/ folders.
# Note: this utility function is **only** designed for use with rythe projects,
# it uses ${RYTHE_DIR_ROOT} to get full filepaths and ${RYTHE_DIR_OUTPUT_INCLUDES} to copy results to.

# Add post-build commands to copy library and header files to output directories.
function(rythe_copy_module_output targetName targetDir)
	# Prepare directories for library output
	add_custom_command(TARGET ${targetName} 
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E
		make_directory ${RYTHE_DIR_OUTPUT_LIBS}/Debug
	)

	add_custom_command(TARGET ${targetName} 
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E
		make_directory ${RYTHE_DIR_OUTPUT_LIBS}/Release
	)

	if (${MODULE_INFO_HAS_SOURCES})
		# Copy debug library
		# Note: || (exit 0) makes sure these commands succeed even if no library file was generated.
		add_custom_command(TARGET ${targetName} 
			POST_BUILD
			COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E 
			copy 
				$<TARGET_FILE:${targetName}>
				"${RYTHE_DIR_OUTPUT_LIBS}/Debug/" || (exit 0)
		)

		# Copy release library
		add_custom_command(TARGET ${targetName} 
			POST_BUILD
			COMMAND $<$<CONFIG:Release>:${CMAKE_COMMAND}> -E 
			copy
				$<TARGET_FILE:${targetName}>
				"${RYTHE_DIR_OUTPUT_LIBS}/Release/" || (exit 0)
		)
	endif()
	
	# Function for recursively going through directories.
	# This is necessary because cmake copy takes a directory,
	# and we want to make sure to copy the files to the correct directory.
	function(read_directory curdir)
		set(SOURCE_PATH ${RYTHE_DIR_ROOT}/${targetDir}/src/${targetName}/${curdir})
		set(DEST_PATH ${RYTHE_DIR_OUTPUT_INCLUDES}/${targetDir}/${curdir})

		# Clear data
		set(files "")

		# Get all files and folders in the current directory
		file(GLOB children 
			LIST_DIRECTORIES true
			RELATIVE ${SOURCE_PATH} 
			${SOURCE_PATH}/*)

		foreach(child ${children})
			# Files can be checked for copying
			if (NOT (IS_DIRECTORY ${SOURCE_PATH}/${child}))
				get_filename_component(CHILD_EXT ${child} EXT)

				# Only copy headers/inl files
				if (CHILD_EXT STREQUAL ".hpp" OR 
					CHILD_EXT STREQUAL ".h" OR 
					CHILD_EXT STREQUAL ".inl")

					# configure_file creates a CMake (configure time) dependency
					# where if the source file changes, the destination file gets updated (in this case copied to)
					configure_file(${SOURCE_PATH}/${child} ${DEST_PATH}/${child} COPYONLY)
				endif()
			# Folders can be recursively looked through for more headers
			else()
				# Skip irrelevant folders
				if (${child} STREQUAL "Debug" OR 
					${child} STREQUAL "Release" OR
					${child} STREQUAL "CMakeFiles" OR 
					${child} STREQUAL "CMakeScripts" OR 
					${child} STREQUAL "${targetName}.dir")
					continue()
				endif()

				# Recursively add directories
				read_directory(${curdir}/${child})
			endif()
		endforeach()
	endfunction()

	# Run the macro on the root folder
	read_directory("")
endfunction()
