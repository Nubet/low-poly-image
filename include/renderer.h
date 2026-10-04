#ifndef LOWPOLY_RENDERER_H
#define LOWPOLY_RENDERER_H

#include "arena.h"
#include "types.h"

Image render_low_poly_image(Arena *arena, const Image *input, const Point *points,
                            const Triangle *triangles, int triangle_count);

#endif
