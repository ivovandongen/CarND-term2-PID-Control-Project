# Submodules
include(${CMAKE_SOURCE_DIR}/deps/uWebSockets.cmake)
include(${CMAKE_SOURCE_DIR}/deps/json.cmake)

add_library(core STATIC
    ${CMAKE_SOURCE_DIR}/src/twiddle.hpp
    ${CMAKE_SOURCE_DIR}/src/PID.hpp
    ${CMAKE_SOURCE_DIR}/src/PID.cpp
    )

target_include_directories(core
    PUBLIC ${CMAKE_SOURCE_DIR}/src
    )

target_link_libraries(core PUBLIC
    libuWS
    json
    )

if (DO_CLANG_TIDY)
    set_target_properties(core PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
endif ()