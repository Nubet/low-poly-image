#include <gtest/gtest.h>

#include <filesystem>

extern "C" {
#include "arena.h"
#include "image.h"
}

TEST(Image, RejectsMissingInputFile)
{
    Arena arena = {};
    Image image = {};

    EXPECT_FALSE(image_load_from_file(&arena, "file-that-does-not-exist.png", &image));
    EXPECT_EQ(image.width, 0);
    EXPECT_EQ(image.height, 0);
    EXPECT_EQ(image.pixels, nullptr);

    arena_free(&arena);
}

TEST(Image, LoadsRepositoryJpeg)
{
    const std::filesystem::path input_path =
        std::filesystem::path(LOWPOLY_TEST_IMAGE_DIR) / "10136-00.jpg";
    Arena arena = {};
    Image image = {};

    ASSERT_TRUE(image_load_from_file(&arena, input_path.string().c_str(), &image));
    EXPECT_EQ(image.width, 1024);
    EXPECT_EQ(image.height, 1024);
    ASSERT_NE(image.pixels, nullptr);

    arena_free(&arena);
}

TEST(Image, SavesAndLoadsPng)
{
    const std::filesystem::path input_path =
        std::filesystem::path(LOWPOLY_TEST_IMAGE_DIR) / "10136-00.jpg";
    const std::filesystem::path output_path =
        std::filesystem::current_path() / "lowpoly-image-test-output.png";
    Arena arena = {};
    Image input = {};
    Image loaded = {};

    ASSERT_TRUE(image_load_from_file(&arena, input_path.string().c_str(), &input));
    ASSERT_TRUE(image_save_as_png(&input, output_path.string().c_str()));
    ASSERT_TRUE(image_load_from_file(&arena, output_path.string().c_str(), &loaded));
    EXPECT_EQ(loaded.width, input.width);
    EXPECT_EQ(loaded.height, input.height);
    EXPECT_EQ(loaded.pixels[0].r, input.pixels[0].r);
    EXPECT_EQ(loaded.pixels[0].g, input.pixels[0].g);
    EXPECT_EQ(loaded.pixels[0].b, input.pixels[0].b);

    arena_free(&arena);
    std::filesystem::remove(output_path);
}
