# Simple dependency macro that checks if the dependency has previously been added before adding it
macro(rythe_add_dependency name dir)
    if (NOT TARGET ${name})
        add_subdirectory(dir)
    endif()
macro()
