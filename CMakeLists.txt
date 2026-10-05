cmake_minimum_required(VERSION 3.21)
project(lowpoly C CXX)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(LOWPOLY_BUILD_GUI "Build the Slint GUI" OFF)

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

set(LOWPOLY_CORE_SOURCES
    src/dependencies.c
    src/geometry.c
    src/image.c
    src/processing.c
    src/random.c
    src/renderer.c
    src/triangulation.c
)

add_library(lowpoly_core STATIC ${LOWPOLY_CORE_SOURCES})
target_compile_options(lowpoly_core PRIVATE -O2 -Wall -Wextra)
target_include_directories(lowpoly_core PUBLIC "${CMAKE_SOURCE_DIR}/include" "${STB_INCLUDE_DIR}")
if(NOT WIN32)
    target_link_libraries(lowpoly_core PUBLIC m)
endif()

add_library(lowpoly_application STATIC src/lowpoly_service.cpp)
target_include_directories(lowpoly_application PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(lowpoly_application PUBLIC lowpoly_core)

add_executable(lowpoly
    src/main.c
    src/options.c
)
target_compile_options(lowpoly PRIVATE -Wall -Wextra)
target_link_libraries(lowpoly PRIVATE lowpoly_core)

enable_testing()
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_executable(lowpoly_tests
    tests/geometry_test.cpp
    tests/image_test.cpp
    tests/lowpoly_service_test.cpp
    tests/options_test.cpp
    tests/processing_test.cpp
    tests/random_test.cpp
    tests/renderer_test.cpp
    tests/triangulation_test.cpp
    src/options.c
)
target_compile_definitions(lowpoly_tests PRIVATE
    LOWPOLY_TEST_IMAGE_DIR="${CMAKE_SOURCE_DIR}/test_images/in"
)
target_link_libraries(lowpoly_tests PRIVATE lowpoly_application GTest::gtest GTest::gtest_main)
gtest_discover_tests(lowpoly_tests)

if(LOWPOLY_BUILD_GUI)
    FetchContent_Declare(
        Slint
        GIT_REPOSITORY https://github.com/slint-ui/slint.git
        GIT_TAG v1.18.1
        SOURCE_SUBDIR api/cpp
    )
    FetchContent_MakeAvailable(Slint)

    add_executable(lowpoly_gui src/gui/main.cpp)
    slint_target_sources(lowpoly_gui ui/main.slint)
    target_link_libraries(lowpoly_gui PRIVATE lowpoly_application Slint::Slint)

    if(WIN32)
        add_custom_command(
            TARGET lowpoly_gui
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:lowpoly_gui>
                    $<TARGET_FILE_DIR:lowpoly_gui>
            COMMAND_EXPAND_LISTS
        )
    endif()
endif()

add_custom_target(run
    COMMAND ${CMAKE_COMMAND}
            -DLOWPOLY_EXECUTABLE=$<TARGET_FILE:lowpoly>
            -DINPUT_DIRECTORY=${CMAKE_SOURCE_DIR}/test_images/in
            -DOUTPUT_DIRECTORY=${CMAKE_SOURCE_DIR}/test_images/out
            -DPOINTS=500
            -DSEED=123
            -P ${CMAKE_SOURCE_DIR}/cmake/generate_outputs.cmake
    DEPENDS lowpoly
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
