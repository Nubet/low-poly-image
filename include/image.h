#ifndef LOWPOLY_IMAGE_H
#define LOWPOLY_IMAGE_H

#include "arena.h"
#include "types.h"

Image image_create_in_arena(Arena *arena, int width, int height);
Image image_load_from_file(Arena *arena, const char *filename);
void image_save_as_png(const Image *image, const char *filename);
Pixel *image_pixel_at(Image *image, int x, int y);
const Pixel *image_pixel_at_const(const Image *image, int x, int y);

#endif
