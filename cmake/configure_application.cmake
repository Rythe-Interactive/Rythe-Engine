# Utility file that introduces rythe_configure_engine_application() and rythe_configure_editor_application()
# These functions simplify setting up compiler settings and dependencies.

function(rythe_configure_engine_application target)
	# Language
	target_compile_features(${target} PUBLIC cxx_std_20)
	
	# Dependency on Rythe
	target_include_directories(${target} PUBLIC ${RYTHE_INCLUDE_ENGINE})
	target_link_libraries(${target} PUBLIC ${RYTHE_LIBS_ENGINE})

	target_include_directories(${target} PUBLIC ${RYTHE_INCLUDE_THIRD_PARTY})
	target_link_libraries(${target} PUBLIC ${RYTHE_LIBS_THIRD_PARTY})
	
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

	message(STATUS "Application: \"${target}\" [Enabled] [Mode=Engine]")
endfunction()
	
function(rythe_configure_editor_application target)
	# Language
	target_compile_features(${target} PUBLIC cxx_std_20)
	
	# Dependency on Rythe
	target_include_directories(${target} PUBLIC ${RYTHE_INCLUDE_ALL})
	target_link_libraries(${target} PUBLIC ${RYTHE_LIBS_ALL})
		
	target_include_directories(${target} PUBLIC ${RYTHE_INCLUDE_THIRD_PARTY})
	target_link_libraries(${target} PUBLIC ${RYTHE_LIBS_THIRD_PARTY})
	
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

	message(STATUS "Application: \"${target}\" [Enabled] [Mode=Editor]")
endfunction()
