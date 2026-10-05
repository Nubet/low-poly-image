#ifndef LOWPOLY_GEOMETRY_H
#define LOWPOLY_GEOMETRY_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

double geometry_calculate_orientation(Point a, Point b, Point c);
int geometry_is_point_inside_triangle(Point p, Point a, Point b, Point c);
Triangle geometry_create_counterclockwise_triangle(int a, int b, int c, Point *points);

#ifdef __cplusplus
}
#endif

#endif
