#include "geometry.h"

double geometry_calculate_orientation(Point a, Point b, Point c)
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

int geometry_is_point_inside_triangle(Point p, Point a, Point b, Point c)
{
    double d1 = geometry_calculate_orientation(a, b, p);
    double d2 = geometry_calculate_orientation(b, c, p);
    double d3 = geometry_calculate_orientation(c, a, p);
    int has_negative = (d1 < 0) || (d2 < 0) || (d3 < 0);
    int has_positive = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_negative && has_positive);
}

Triangle geometry_create_counterclockwise_triangle(int a, int b, int c, Point *points)
{
    Triangle triangle = {a, b, c};

    if (geometry_calculate_orientation(points[a], points[b], points[c]) < 0) {
        int temp = triangle.b;
        triangle.b = triangle.c;
        triangle.c = temp;
    }

    return triangle;
}
