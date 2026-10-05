#include <gtest/gtest.h>

#include <filesystem>
#include <string>

extern "C" {
#include "arena.h"
#include "processing.h"
}

TEST(Processing, GeneratesOutputFromInput)
{
    const std::string input_path =
        (std::filesystem::path(LOWPOLY_TEST_IMAGE_DIR) / "10136-00.jpg").string();
    Arena arena = {};
    LowpolyProcessRequest request = {
        .input_path = input_path.c_str(),
        .point_count = 20,
        .seed = 123,
    };
    LowpolyProcessResult result = {};

    EXPECT_EQ(lowpoly_process(&arena, &request, &result), LOWPOLY_PROCESS_SUCCESS);
    EXPECT_EQ(result.output.width, 1024);
    EXPECT_EQ(result.output.height, 1024);
    EXPECT_GT(result.triangle_count, 0);
    ASSERT_NE(result.output.pixels, nullptr);

    arena_free(&arena);
}

TEST(Processing, RejectsInvalidRequest)
{
    Arena arena = {};
    LowpolyProcessRequest request = {
        .input_path = "input.jpg",
        .point_count = LOWPOLY_MIN_POINT_COUNT - 1,
        .seed = 123,
    };
    LowpolyProcessResult result = {};

    EXPECT_EQ(lowpoly_process(&arena, &request, &result), LOWPOLY_PROCESS_INVALID_REQUEST);
    arena_free(&arena);
}
