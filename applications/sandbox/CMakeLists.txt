# Sandbox depends on applications and graphics in this case

if (${RYTHE_BUILD_APPLICATIONS} AND ${RYTHE_MODULE_GRAPHICS})
	project(sandbox LANGUAGES ${RYTHE_LANGUAGE})

	rythe_find_sources(sources "${PROJECT_SOURCE_DIR}")
	add_executable(sandbox ${sources})
	set_target_properties(sandbox PROPERTIES FOLDER "2 - applications")

	rythe_configure_engine_application(sandbox)
	
	list(APPEND RYTHE_ENABLED_APPLICATION_NAMES "sandbox")
	set(RYTHE_ENABLED_APPLICATION_NAMES ${RYTHE_ENABLED_APPLICATION_NAMES} PARENT_SCOPE)
endif()
