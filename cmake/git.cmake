# Update submodules as needed
option(GIT_SUBMODULE "Check submodules during build" OFF)

if (GIT_SUBMODULE)

    # First try preferred GIT
    if (GIT_EXECUTABLE)
        if (EXISTS ${GIT_EXECUTABLE})
            set(GIT_FOUND TRUE)
        else()
            message(STATUS "GIT: Could not find git at specified location: ${GIT_EXECUTABLE}")
            unset(GIT_EXECUTABLE CACHE)
            set(GIT_FOUND FALSE)
        endif()
    endif()

    # Try with find_package
    if (NOT GIT_FOUND)
        unset(GIT_EXECUTABLE CACHE)
        message(STATUS "GIT: Trying to location with find_package")
        find_package(Git QUIET)
    endif()

    # Try with find_program
    if (NOT GIT_FOUND)
        unset(GIT_EXECUTABLE CACHE)
        message(STATUS "GIT: Trying to location with find_program")
        find_program(GIT_EXECUTABLE git)
        if (EXISTS ${GIT_EXECUTABLE})
            set(GIT_FOUND TRUE)
        endif()
    endif()


    # Try some alternate paths for macos installs
    if (NOT GIT_FOUND)
        unset(GIT_EXECUTABLE CACHE)
        message(STATUS "GIT: git not found. Trying alternatives")
        if (EXISTS "/usr/local/bin/git")
            set(GIT_EXECUTABLE "/usr/local/bin/git")
            set(GIT_FOUND TRUE)
        elseif(EXISTS "/usr/bin/git")
            set(GIT_EXECUTABLE "/usr/bin/git")
            set(GIT_FOUND TRUE)
        else()
            message(STATUS "GIT: No alternative path could be found for git")
        endif()
    endif()

    if(GIT_SUBMODULE AND GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
        message(STATUS "GIT: Using git from ${GIT_EXECUTABLE}")
        if(${GIT_SUBMODULE})
            message(STATUS "GIT: Submodule update")
            execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT)
            if(NOT GIT_SUBMOD_RESULT EQUAL "0")
                message(FATAL_ERROR "GIT: git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
            endif()
        else()
            message(STATUS "GIT: No git submodules found")
        endif()
    elseif(GIT_SUBMODULE)
        message(FATAL_ERROR "GIT: Could not find git
        Try setting the path to the executable with -DGIT_EXECUTABLE=/path/to/git.

        ALTERNATIVE: To initialize submodules manually instead, run git submodule update --init --recursive
        and then re-run cmake with -DGIT_SUBMODULE=OFF
        ")
    else()
        message(STATUS "GIT: Assuming submodules are manually initialized")
    endif()

endif()