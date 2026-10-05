#ifndef LOWPOLY_TRIANGULATION_H
#define LOWPOLY_TRIANGULATION_H

#include "arena.h"
#include "random.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

Point *generate_random_points_with_corners(Arena *arena, int width, int height, int requested_count,
                                           Random *random);

Triangle *build_delaunay_triangulation(Arena *arena, Point *points, int real_point_count,
                                       int *out_triangle_count);

#ifdef __cplusplus
}
#endif

#endif
