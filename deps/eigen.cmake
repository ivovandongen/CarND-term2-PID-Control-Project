add_library(Eigen INTERFACE)

target_include_directories(Eigen INTERFACE ${CMAKE_SOURCE_DIR}/deps/Eigen/Eigen)

if (UNIX)
    # Silence warnings in Eigen
    add_compile_options(-Wno-deprecated-register)
endif()