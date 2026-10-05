#include <gtest/gtest.h>

extern "C" {
#include "geometry.h"
}

TEST(Geometry, CalculatesOrientation)
{
    EXPECT_GT(geometry_calculate_orientation({0, 0}, {1, 0}, {0, 1}), 0.0);
    EXPECT_LT(geometry_calculate_orientation({0, 0}, {0, 1}, {1, 0}), 0.0);
    EXPECT_DOUBLE_EQ(geometry_calculate_orientation({0, 0}, {1, 1}, {2, 2}), 0.0);
}

TEST(Geometry, DetectsPointsInsideTriangle)
{
    Point a = {0, 0};
    Point b = {10, 0};
    Point c = {0, 10};

    EXPECT_TRUE(geometry_is_point_inside_triangle({1, 1}, a, b, c));
    EXPECT_TRUE(geometry_is_point_inside_triangle({0, 0}, a, b, c));
    EXPECT_FALSE(geometry_is_point_inside_triangle({8, 8}, a, b, c));
}

TEST(Geometry, IncludesPointsOnEveryEdge)
{
    Point a = {0, 0};
    Point b = {10, 0};
    Point c = {0, 10};

    EXPECT_TRUE(geometry_is_point_inside_triangle({5, 0}, a, b, c));
    EXPECT_TRUE(geometry_is_point_inside_triangle({0, 5}, a, b, c));
    EXPECT_TRUE(geometry_is_point_inside_triangle({5, 5}, a, b, c));
}

TEST(Geometry, HandlesClockwiseTriangle)
{
    Point a = {0, 0};
    Point b = {0, 10};
    Point c = {10, 0};

    EXPECT_TRUE(geometry_is_point_inside_triangle({1, 1}, a, b, c));
    EXPECT_FALSE(geometry_is_point_inside_triangle({8, 8}, a, b, c));
}

TEST(Geometry, CreatesCounterclockwiseTriangle)
{
    Point points[] = {{0, 0}, {0, 1}, {1, 0}};
    Triangle triangle = geometry_create_counterclockwise_triangle(0, 1, 2, points);

    EXPECT_GT(
        geometry_calculate_orientation(points[triangle.a], points[triangle.b], points[triangle.c]),
        0.0);
}
