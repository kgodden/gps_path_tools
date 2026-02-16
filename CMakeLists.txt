cmake_minimum_required(VERSION 3.15)
project(path_tools_examples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "CMake build type: ${CMAKE_BUILD_TYPE}")

add_executable(path_tools_tests
    test/test_gps_path_tools.cpp
)

add_executable(path_tools_examples
    examples/path_tools_examples.cpp
)

if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "")
    set(CMAKE_MODULE_LINKER_FLAGS "")
    set(CMAKE_SHARED_LINKER_FLAGS "")
    set(CMAKE_CXX_STANDARD_LIBRARIES "")

    set_target_properties(path_tools_examples PROPERTIES
        LINK_FLAGS "-static-libgcc"
    )
    set_target_properties(path_tools_tests PROPERTIES
        LINK_FLAGS "-static-libgcc"
    )

endif()

target_compile_definitions(path_tools_examples PRIVATE _USE_MATH_DEFINES)