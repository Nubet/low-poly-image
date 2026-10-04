cmake_minimum_required(VERSION 3.20)
project(lowpoly C)

set(STB_INCLUDE_DIR "${CMAKE_BINARY_DIR}/generated/stb")
file(MAKE_DIRECTORY "${STB_INCLUDE_DIR}")

file(
    DOWNLOAD
    https://raw.githubusercontent.com/nothings/stb/2c980bb59875b0d32144a71867fbdebb2f77cd20/stb_image.h
    "${STB_INCLUDE_DIR}/stb_image.h"
    EXPECTED_HASH SHA256=594c2fe35d49488b4382dbfaec8f98366defca819d916ac95becf3e75f4200b3
)

file(
    DOWNLOAD
    https://raw.githubusercontent.com/nothings/stb/2c980bb59875b0d32144a71867fbdebb2f77cd20/stb_image_write.h
    "${STB_INCLUDE_DIR}/stb_image_write.h"
    EXPECTED_HASH SHA256=cbd5f0ad7a9cf4468affb36354a1d2338034f2c12473cf1a8e32053cb6914a05
)

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
target_include_directories(lowpoly PRIVATE "${CMAKE_SOURCE_DIR}/include" "${STB_INCLUDE_DIR}")
if(NOT WIN32)
    target_link_libraries(lowpoly PRIVATE m)
endif()

add_custom_target(run
    COMMAND lowpoly "${CMAKE_SOURCE_DIR}/input.png" "${CMAKE_BINARY_DIR}/output.png" 500
    DEPENDS lowpoly
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)
