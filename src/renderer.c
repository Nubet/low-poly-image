#include "renderer.h"

#include <math.h>

#include "geometry.h"
#include "image.h"

static void rasterize_filled_triangle(Image *image, Point a, Point b, Point c, Pixel color)
{
    int min_x = (int)floor(fmin(a.x, fmin(b.x, c.x)));
    int max_x = (int)ceil(fmax(a.x, fmax(b.x, c.x)));
    int min_y = (int)floor(fmin(a.y, fmin(b.y, c.y)));
    int max_y = (int)ceil(fmax(a.y, fmax(b.y, c.y)));

    if (min_x < 0)
        min_x = 0;
    if (min_y < 0)
        min_y = 0;
    if (max_x >= image->width)
        max_x = image->width - 1;
    if (max_y >= image->height)
        max_y = image->height - 1;

    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            Point pixel = {x + 0.5, y + 0.5};

            if (geometry_is_point_inside_triangle(pixel, a, b, c)) {
                *image_pixel_at(image, x, y) = color;
            }
        }
    }
}

static Pixel sample_color_at_triangle_centroid(const Image *input, Point a, Point b, Point c)
{
    int x = (int)((a.x + b.x + c.x) / 3.0);
    int y = (int)((a.y + b.y + c.y) / 3.0);

    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;
    if (x >= input->width)
        x = input->width - 1;
    if (y >= input->height)
        y = input->height - 1;

    return *image_pixel_at_const(input, x, y);
}

Image render_low_poly_image(Arena *arena, const Image *input, const Point *points,
                            const Triangle *triangles, int triangle_count)
{
    Image output = image_create_in_arena(arena, input->width, input->height);

    for (int i = 0; i < triangle_count; i++) {
        Triangle triangle = triangles[i];
        Point a = points[triangle.a];
        Point b = points[triangle.b];
        Point c = points[triangle.c];
        Pixel color = sample_color_at_triangle_centroid(input, a, b, c);

        rasterize_filled_triangle(&output, a, b, c, color);
    }

    return output;
}
