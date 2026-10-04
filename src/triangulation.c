#include "triangulation.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "geometry.h"

static int edges_are_equivalent(Edge a, Edge b)
{
    return (a.a == b.a && a.b == b.b) || (a.a == b.b && a.b == b.a);
}

static void add_or_remove_edge(Edge *edges, int *edge_count, Edge edge)
{
    for (int i = 0; i < *edge_count; i++) {
        if (edges_are_equivalent(edges[i], edge)) {
            edges[i] = edges[*edge_count - 1];
            (*edge_count)--;
            return;
        }
    }

    edges[*edge_count] = edge;
    (*edge_count)++;
}

static int is_point_inside_circumcircle(Point a, Point b, Point c, Point p)
{
    double ax = a.x - p.x;
    double ay = a.y - p.y;
    double bx = b.x - p.x;
    double by = b.y - p.y;
    double cx = c.x - p.x;
    double cy = c.y - p.y;

    double determinant = (ax * ax + ay * ay) * (bx * cy - cx * by) -
                         (bx * bx + by * by) * (ax * cy - cx * ay) +
                         (cx * cx + cy * cy) * (ax * by - bx * ay);

    return determinant > 0.0;
}

Point *generate_random_points_with_corners(Arena *arena, int width, int height, int requested_count)
{
    Point *points = arena_alloc(arena, sizeof(Point) * (requested_count + 3));

    points[0] = (Point){0, 0};
    points[1] = (Point){width - 1, 0};
    points[2] = (Point){width - 1, height - 1};
    points[3] = (Point){0, height - 1};

    for (int i = 4; i < requested_count; i++) {
        points[i].x = rand() % width;
        points[i].y = rand() % height;
    }

    return points;
}

Triangle *build_delaunay_triangulation(Arena *arena, Point *points, int real_point_count,
                                       int *out_triangle_count)
{
    double min_x = points[0].x;
    double max_x = points[0].x;
    double min_y = points[0].y;
    double max_y = points[0].y;

    for (int i = 1; i < real_point_count; i++) {
        if (points[i].x < min_x)
            min_x = points[i].x;
        if (points[i].x > max_x)
            max_x = points[i].x;
        if (points[i].y < min_y)
            min_y = points[i].y;
        if (points[i].y > max_y)
            max_y = points[i].y;
    }

    double diameter = fmax(max_x - min_x, max_y - min_y);
    double center_x = (min_x + max_x) / 2.0;
    double center_y = (min_y + max_y) / 2.0;
    int s0 = real_point_count;
    int s1 = real_point_count + 1;
    int s2 = real_point_count + 2;

    points[s0] = (Point){center_x, center_y - 20.0 * diameter};
    points[s1] = (Point){center_x - 20.0 * diameter, center_y + 20.0 * diameter};
    points[s2] = (Point){center_x + 20.0 * diameter, center_y + 20.0 * diameter};

    int triangle_capacity = real_point_count * 10 + 100;
    Triangle *triangles = arena_alloc(arena, sizeof(Triangle) * triangle_capacity);
    int triangle_count = 1;

    triangles[0] = geometry_create_counterclockwise_triangle(s0, s1, s2, points);

    for (int p = 0; p < real_point_count; p++) {
        Arena_Mark scratch = arena_snapshot(arena);
        Edge *polygon = arena_alloc(arena, sizeof(Edge) * triangle_capacity * 3);
        int polygon_count = 0;

        for (int i = 0; i < triangle_count;) {
            Triangle triangle = triangles[i];

            if (is_point_inside_circumcircle(points[triangle.a], points[triangle.b],
                                             points[triangle.c], points[p])) {
                add_or_remove_edge(polygon, &polygon_count, (Edge){triangle.a, triangle.b});
                add_or_remove_edge(polygon, &polygon_count, (Edge){triangle.b, triangle.c});
                add_or_remove_edge(polygon, &polygon_count, (Edge){triangle.c, triangle.a});
                triangles[i] = triangles[triangle_count - 1];
                triangle_count--;
            } else {
                i++;
            }
        }

        for (int i = 0; i < polygon_count; i++) {
            if (triangle_count >= triangle_capacity) {
                fprintf(stderr, "triangle capacity exceeded\n");
                exit(1);
            }

            triangles[triangle_count++] =
                geometry_create_counterclockwise_triangle(polygon[i].a, polygon[i].b, p, points);
        }

        arena_rewind(arena, scratch);
    }

    for (int i = 0; i < triangle_count;) {
        Triangle triangle = triangles[i];

        if (triangle.a >= real_point_count || triangle.b >= real_point_count ||
            triangle.c >= real_point_count) {
            triangles[i] = triangles[triangle_count - 1];
            triangle_count--;
        } else {
            i++;
        }
    }

    *out_triangle_count = triangle_count;
    return triangles;
}
