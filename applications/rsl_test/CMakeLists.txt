project(rsl_test LANGUAGES ${RYTHE_LANGUAGE})

rythe_find_sources(sources "${PROJECT_SOURCE_DIR}")
add_executable(rsl_test ${sources})
set_target_properties(rsl_test PROPERTIES FOLDER "2 - applications")

# Dependency on RSL
target_include_directories(rsl_test PUBLIC "${RYTHE_DIR_ENGINE}/core/third_party/rythe-standard-library/src")
target_link_libraries(rsl_test PUBLIC rythe-standard-library)

rythe_configure_manual_application(rsl_test)
	
set(RYTHE_ENABLED_APPLICATION_NAMES ${RYTHE_ENABLED_APPLICATION_NAMES} PARENT_SCOPE)
