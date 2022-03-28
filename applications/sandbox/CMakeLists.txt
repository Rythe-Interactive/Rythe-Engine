# Sandbox depends on applications and graphics in this case

if (${RYTHE_MODULE_APPLICATION} AND ${RYTHE_MODULE_GRAPHICS})
	project(sandbox LANGUAGES ${RYTHE_LANGUAGE})

	rythe_find_sources(sources "${PROJECT_SOURCE_DIR}")
	add_executable(sandbox ${sources})
	set_target_properties(sandbox PROPERTIES FOLDER applications)

	rythe_configure_engine_application(sandbox)
endif()
