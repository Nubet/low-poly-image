#include <gtest/gtest.h>

extern "C" {
#include "arena.h"
#include "random.h"
#include "triangulation.h"
}

TEST(Triangulation, IncludesImageCorners)
{
    Arena arena = {};
    Random random = {};
    random_seed(&random, 123);
    Point *points = generate_random_points_with_corners(&arena, 20, 30, 4, &random);

    ASSERT_NE(points, nullptr);
    EXPECT_DOUBLE_EQ(points[0].x, 0.0);
    EXPECT_DOUBLE_EQ(points[0].y, 0.0);
    EXPECT_DOUBLE_EQ(points[1].x, 19.0);
    EXPECT_DOUBLE_EQ(points[1].y, 0.0);
    EXPECT_DOUBLE_EQ(points[2].x, 19.0);
    EXPECT_DOUBLE_EQ(points[2].y, 29.0);
    EXPECT_DOUBLE_EQ(points[3].x, 0.0);
    EXPECT_DOUBLE_EQ(points[3].y, 29.0);

    arena_free(&arena);
}

TEST(Triangulation, BuildsValidDelaunayMesh)
{
    Arena arena = {};
    Random random = {};
    random_seed(&random, 123);
    constexpr int point_count = 25;
    Point *points = generate_random_points_with_corners(&arena, 100, 80, point_count, &random);
    int triangle_count = 0;

    Triangle *triangles =
        build_delaunay_triangulation(&arena, points, point_count, &triangle_count);

    ASSERT_NE(triangles, nullptr);
    EXPECT_GT(triangle_count, 0);
    for (int i = 0; i < triangle_count; ++i) {
        EXPECT_GE(triangles[i].a, 0);
        EXPECT_LT(triangles[i].a, point_count);
        EXPECT_GE(triangles[i].b, 0);
        EXPECT_LT(triangles[i].b, point_count);
        EXPECT_GE(triangles[i].c, 0);
        EXPECT_LT(triangles[i].c, point_count);
        EXPECT_NE(triangles[i].a, triangles[i].b);
        EXPECT_NE(triangles[i].b, triangles[i].c);
        EXPECT_NE(triangles[i].a, triangles[i].c);

        double orientation = (points[triangles[i].b].x - points[triangles[i].a].x) *
                                 (points[triangles[i].c].y - points[triangles[i].a].y) -
                             (points[triangles[i].b].y - points[triangles[i].a].y) *
                                 (points[triangles[i].c].x - points[triangles[i].a].x);
        EXPECT_GT(orientation, 0.0);
    }

    for (int triangle_index = 0; triangle_index < triangle_count; ++triangle_index) {
        const Triangle triangle = triangles[triangle_index];
        const Point a = points[triangle.a];
        const Point b = points[triangle.b];
        const Point c = points[triangle.c];

        for (int point_index = 0; point_index < point_count; ++point_index) {
            if (point_index == triangle.a || point_index == triangle.b || point_index == triangle.c)
                continue;

            const Point p = points[point_index];
            const double ax = a.x - p.x;
            const double ay = a.y - p.y;
            const double bx = b.x - p.x;
            const double by = b.y - p.y;
            const double cx = c.x - p.x;
            const double cy = c.y - p.y;
            const double determinant = (ax * ax + ay * ay) * (bx * cy - cx * by) -
                                       (bx * bx + by * by) * (ax * cy - cx * ay) +
                                       (cx * cx + cy * cy) * (ax * by - bx * ay);

            EXPECT_LE(determinant, 1e-7)
                << "point " << point_index << " is inside triangle " << triangle_index;
        }
    }

    arena_free(&arena);
}

TEST(Triangulation, ProducesTwoTrianglesForImageRectangle)
{
    Arena arena = {};
    Random random = {};
    random_seed(&random, 123);
    Point *points = generate_random_points_with_corners(&arena, 20, 30, 4, &random);
    int triangle_count = 0;

    Triangle *triangles = build_delaunay_triangulation(&arena, points, 4, &triangle_count);

    ASSERT_NE(triangles, nullptr);
    EXPECT_EQ(triangle_count, 2);
    arena_free(&arena);
}
