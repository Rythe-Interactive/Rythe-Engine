# Utility file that defines the post_build_copy() function.
# post_build_copy adds post-build commands to targets that copy library and header files,
# to the designated rythe include/ and lib/ folders.
# Note: this utility function is **only** designed for use with rythe projects,
# it uses ${RYTHE_DIR_ROOT} to get full filepaths and ${RYTHE_DIR_OUTPUT_INCLUDES} to copy results to.

# Util function for internal use in post_build_copy.
# Adds a post-build command to target that creates a directory at path p.
function(create_dir target p)
	add_custom_command(TARGET ${target} 
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E
		make_directory ${p}
	)
endfunction()

# Util function for internal use in post_build_copy.
# Adds a post-build command to target that deletes a directory at path p.
function(delete_dir target p)
	add_custom_command(TARGET ${target} 
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E 
		remove_directory  ${p}
	)
endfunction()

# Add post-build commands to copy library and header files to output directories.
function(post_build_copy targetName targetDir)
	# Prepare directories for library output
	create_dir(${targetName} ${RYTHE_OUTPUT_LIB_DIR}/Debug)
	create_dir(${targetName} ${RYTHE_OUTPUT_LIB_DIR}/Release)

	# Copy debug library
	add_custom_command(TARGET ${targetName} 
		POST_BUILD
		COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E 
		copy 
			$<TARGET_FILE:${targetName}>
			"${RYTHE_DIR_OUTPUT_LIBS}/Debug/"
	)

	# Copy release library
	add_custom_command(TARGET ${targetName} 
		POST_BUILD
		COMMAND $<$<CONFIG:Release>:${CMAKE_COMMAND}> -E 
		copy
			$<TARGET_FILE:${targetName}>
			"${RYTHE_DIR_OUTPUT_LIBS}/Release/"
	)

	# Clear previously created output directory for header files
	delete_dir(${targetName} ${RYTHE_OUTPUT_INCLUDE_DIR}/${targetDir}/)

	# Macro for recursively going through directories.
	# This is necessary because cmake copy takes a directory,
	# and we want to make sure to copy the files to the correct directory.
	macro(read_directory curdir)
		# Get all files and folders in the current directory
		file(GLOB children RELATIVE ${RYTHE_ROOT_DIR}/${curdir}/ ${RYTHE_ROOT_DIR}/${curdir}/*)
		
		# Clear files list
		set(files "")

		foreach(child ${children})
			# Files can be added to our files list for copying
			if (NOT (IS_DIRECTORY ${RYTHE_ROOT_DIR}/${curdir}/${child}))
				get_filename_component(CHILD_EXT ${child} EXT)

				# Only copy headers/inl files
				if (CHILD_EXT STREQUAL ".hpp" OR 
					CHILD_EXT STREQUAL ".h" OR 
					CHILD_EXT STREQUAL ".inl")
					list(APPEND files ${RYTHE_ROOT_DIR}/${curdir}/${child})
				endif()
			# Folders can be recursively looked through for more headers
			else()
				# Skip irrelevant folders
				if (${child} STREQUAL "Debug" OR 
					${child} STREQUAL "Release" OR
					${child} STREQUAL "CMakeFiles" OR 
					${child} STREQUAL "${targetName}.dir")
					continue()
				endif()

				# Recursively add directories
				read_directory(${curdir}/${child})
			endif()
		endforeach()

		# Prepare the output directory for copying
		create_dir(${targetName} ${RYTHE_DIR_OUTPUT_INCLUDES}/${curdir})

		# Add post-build command that copies the selected files to the dedicated output folder
		add_custom_command(TARGET ${targetName}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E 
			copy
				${files}
				${RYTHE_DIR_OUTPUT_INCLUDES}/${curdir}/
		)
	endmacro()

	# Run the macro on the root folder
	read_directory(${targetDir})
endfunction()