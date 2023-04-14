# Sandbox depends on applications and graphics in this case

if (${RYTHE_BUILD_APPLICATIONS} AND ${RYTHE_MODULE_GRAPHICS})
	project(sandbox LANGUAGES ${RYTHE_LANGUAGE})

	rythe_find_sources(sources "${PROJECT_SOURCE_DIR}")
	add_executable(sandbox ${sources})
	set_target_properties(sandbox PROPERTIES FOLDER "2 - applications")

	target_include_directories(sandbox PUBLIC "${RYTHE_DIR_ENGINE}/core/third_party/rythe-standard-library/src")
	target_link_libraries(sandbox PUBLIC rythe-standard-library)

	target_include_directories(sandbox PUBLIC "${RYTHE_DIR_ENGINE}/core/src")
	target_link_libraries(sandbox PUBLIC core)

	rythe_configure_manual_application(sandbox)
	
	set(RYTHE_ENABLED_APPLICATION_NAMES ${RYTHE_ENABLED_APPLICATION_NAMES} PARENT_SCOPE)
endif()
