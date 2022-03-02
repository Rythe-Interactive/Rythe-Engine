# Utility file to be included directly into a CMake file.
# Uses `git submodule` to get a list of submodule names, paths, and commit hashes.
#
# `git submodule` outputs every submodule in the repo (non-recursively), separated by line e.g.:
# d8aab41087704f9e7e69f473bdd439fd446bb102 rythe/engine/modules/rendering (remotes/origin/HEAD)
# We can simply split the output by line, and then by spaces, after which we can read out the path,
# and get the text after the last / to get the submodule's name.
#
# output ${SUBMODULE_NAMES}     list of submodule names (last part of the path, so "path/to/submodule" is just "submodule")
# output ${SUBMODULE_PATHS}     list of submodule paths (from root, "path/to/submodule")
# output ${SUBMODULE_HASHES}    list of submodule commit hashes
# output ${SUBMODULE_COUNT}     number of submodules in the lists

# Clear output
set(SUBMODULE_NAMES "")
set(SUBMODULE_PATHS "")
set(SUBMODULE_HASHES "")
set(SUBMODULE_COUNT 0)

# Get submodules
execute_process(COMMAND ${GIT_EXECUTABLE} submodule
    WORKING_DIRECTORY ${RYTHE_DIR_ROOT}
    RESULT_VARIABLE GIT_SUBMOD_RESULT
    OUTPUT_VARIABLE GIT_SUBMOD_OUTPUT)
if (NOT GIT_SUBMOD_RESULT EQUAL "0")
    message(FATAL_ERROR "git submodule failed with ${GIT_SUBMOD_RESULT}")
endif()

if (NOT GIT_SUBMOD_OUTPUT STREQUAL "")
    # Split into lines
    string(REPLACE "\n" ";" SUBMODULES_LINES ${GIT_SUBMOD_OUTPUT})

    foreach(LINE ${SUBMODULES_LINES})
        # Remove starting/trailing whitespace
        string(STRIP "${LINE}" LINE)

        # Split by space to get the hash and path
        string(REPLACE " " ";" LINE_ENTRIES ${LINE})
        list(GET LINE_ENTRIES 0 SUBMODULE_HASH)
        list(GET LINE_ENTRIES 1 SUBMODULE_PATH)

        # Get the submodule name by splitting the path by / and getting the last element
        string(REPLACE "/" ";" SUBMODULE_PATH_PARTS "${SUBMODULE_PATH}")
        list(GET SUBMODULE_PATH_PARTS -1 SUBMODULE_NAME)

        # Skip notfound
        if (${SUBMODULE_NAME} STREQUAL "notfound")
            continue()
        endif()

        list(APPEND SUBMODULE_NAMES ${SUBMODULE_NAME})
        list(APPEND SUBMODULE_PATHS ${SUBMODULE_PATH})
        list(APPEND SUBMODULE_HASHES ${SUBMODULE_HASH})
    endforeach()

    # Convenient SUBMODULE_COUNT variable, can be used for iteration 
    list(LENGTH SUBMODULE_NAMES SUBMODULE_COUNT)
    
endif()
