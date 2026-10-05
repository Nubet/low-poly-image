#include <gtest/gtest.h>

extern "C" {
#include "arena.h"
#include "image.h"
#include "renderer.h"
}

TEST(Renderer, FillsTriangleWithCentroidSample)
{
    Arena arena = {};
    Image input = image_create_in_arena(&arena, 4, 4);
    for (int y = 0; y < input.height; ++y) {
        for (int x = 0; x < input.width; ++x)
            *image_pixel_at(&input, x, y) = {0, 0, 0};
    }
    *image_pixel_at(&input, 1, 1) = {10, 20, 30};

    Point points[] = {{0, 0}, {3, 0}, {0, 3}};
    Triangle triangles[] = {{0, 1, 2}};
    Image output = render_low_poly_image(&arena, &input, points, triangles, 1);

    EXPECT_EQ(output.width, 4);
    EXPECT_EQ(output.height, 4);
    Pixel filled = *image_pixel_at(&output, 0, 0);
    EXPECT_EQ(filled.r, 10);
    EXPECT_EQ(filled.g, 20);
    EXPECT_EQ(filled.b, 30);

    Pixel untouched = *image_pixel_at(&output, 3, 3);
    EXPECT_EQ(untouched.r, 0);
    EXPECT_EQ(untouched.g, 0);
    EXPECT_EQ(untouched.b, 0);

    arena_free(&arena);
}
