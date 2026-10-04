#ifndef LOWPOLY_TRIANGULATION_H
#define LOWPOLY_TRIANGULATION_H

#include "arena.h"
#include "types.h"

Point *generate_random_points_with_corners(Arena *arena, int width, int height,
                                           int requested_count);

Triangle *build_delaunay_triangulation(Arena *arena, Point *points, int real_point_count,
                                       int *out_triangle_count);

#endif
