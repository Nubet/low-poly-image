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
