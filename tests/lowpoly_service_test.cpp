#include <gtest/gtest.h>

#include <filesystem>
#include <string>

#include "lowpoly_service.hpp"

TEST(LowpolyService, ReturnsOwnedRgbImage)
{
    const std::string input_path =
        (std::filesystem::path(LOWPOLY_TEST_IMAGE_DIR) / "10136-00.jpg").string();
    lowpoly::LowpolyService service;
    lowpoly::ProcessRequest request = {
        .input_path = input_path,
        .point_count = 20,
        .seed = 123,
    };

    lowpoly::ProcessResponse response = service.process(request);

    ASSERT_TRUE(response.succeeded());
    EXPECT_EQ(response.width, 1024);
    EXPECT_EQ(response.height, 1024);
    EXPECT_GT(response.triangle_count, 0);
    EXPECT_EQ(response.rgb_pixels.size(), static_cast<size_t>(1024 * 1024 * 3));
}

TEST(LowpolyService, MapsInvalidRequestStatus)
{
    lowpoly::LowpolyService service;
    lowpoly::ProcessRequest request = {
        .input_path = "input.jpg",
        .point_count = 3,
        .seed = 123,
    };

    lowpoly::ProcessResponse response = service.process(request);

    EXPECT_EQ(response.status, lowpoly::ProcessStatus::invalid_request);
    EXPECT_TRUE(response.rgb_pixels.empty());
}

TEST(LowpolyService, SavesOwnedRgbImageAsPngAndJpeg)
{
    const std::string input_path =
        (std::filesystem::path(LOWPOLY_TEST_IMAGE_DIR) / "10136-00.jpg").string();
    const std::filesystem::path output_path =
        std::filesystem::temp_directory_path() / "lowpoly_service_test.png";
    const std::filesystem::path jpeg_output_path =
        std::filesystem::temp_directory_path() / "lowpoly_service_test.jpg";
    std::error_code error;
    std::filesystem::remove(output_path, error);
    std::filesystem::remove(jpeg_output_path, error);

    lowpoly::LowpolyService service;
    lowpoly::ProcessRequest request = {
        .input_path = input_path,
        .point_count = 20,
        .seed = 123,
    };
    const lowpoly::ProcessResponse response = service.process(request);

    ASSERT_TRUE(response.succeeded());
    EXPECT_TRUE(service.save_image(response, output_path.string(), lowpoly::ImageFormat::png));
    EXPECT_TRUE(std::filesystem::exists(output_path));

    EXPECT_TRUE(
        service.save_image(response, jpeg_output_path.string(), lowpoly::ImageFormat::jpeg));
    EXPECT_TRUE(std::filesystem::exists(jpeg_output_path));

    std::filesystem::remove(output_path, error);
    std::filesystem::remove(jpeg_output_path, error);
}
