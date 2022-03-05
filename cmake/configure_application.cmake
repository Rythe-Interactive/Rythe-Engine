# Utility file that introduces configure_engine_application() and configure_editor_application()
# These functions simplify setting up compiler settings and dependencies.

function(configure_engine_application target)
	# Language
	target_compile_features(${target} PUBLIC cxx_std_17)
	
	# Dependency on Rythe
	target_include_directories(${target} PUBLIC ${RYTHE_INCLUDE_ENGINE})
	target_link_libraries(${target} PUBLIC ${RYTHE_LIBS_ENGINE})
	
	# Compiler
	target_compile_definitions(${target} PUBLIC ${RYTHE_DEFINITIONS})
	target_compile_options(${target} PUBLIC ${RYTHE_COMPILER_FLAGS})
	
	# Linker
	if (NOT ${RYTHE_LINKER_FLAGS} STREQUAL "")
		target_link_options(${library} PUBLIC ${RYTHE_LINKER_FLAGS})
	endif()
	set_target_properties(${library} PROPERTIES LINKER_LANGUAGE ${RYTHE_LANGUAGE})
endfunction()
	
function(configure_editor_application target)
	# Language
	target_compile_features(${target} PUBLIC cxx_std_17)
	
	# Dependency on Rythe
	target_include_directories(${target} PUBLIC ${RYTHE_INCLUDE_ALL})
	target_link_libraries(${target} PUBLIC ${RYTHE_LIBS_ALL})
	
	# Compiler
	target_compile_definitions(${target} PUBLIC ${RYTHE_DEFINITIONS})
	target_compile_options(${target} PUBLIC ${RYTHE_COMPILER_FLAGS})
	
	# Linker
	if (NOT ${RYTHE_LINKER_FLAGS} STREQUAL "")
		target_link_options(${library} PUBLIC ${RYTHE_LINKER_FLAGS})
	endif()
	set_target_properties(${library} PROPERTIES LINKER_LANGUAGE ${RYTHE_LANGUAGE})
endfunction()
