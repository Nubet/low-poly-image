cmake_minimum_required(VERSION 3.20)
project(lowpoly C)

include(FetchContent)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG 2c980bb59875b0d32144a71867fbdebb2f77cd20
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(stb)

add_executable(
    lowpoly
    src/dependencies.c
    src/geometry.c
    src/image.c
    src/main.c
    src/renderer.c
    src/triangulation.c
)
target_compile_options(lowpoly PRIVATE -O2 -Wall -Wextra)
target_include_directories(lowpoly PRIVATE "${CMAKE_SOURCE_DIR}/include" "${stb_SOURCE_DIR}")
if(NOT WIN32)
    target_link_libraries(lowpoly PRIVATE m)
endif()

add_custom_target(run
    COMMAND lowpoly "${CMAKE_SOURCE_DIR}/input.png" "${CMAKE_BINARY_DIR}/output.png" 500
    DEPENDS lowpoly
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)
