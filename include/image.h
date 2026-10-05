#ifndef LOWPOLY_IMAGE_H
#define LOWPOLY_IMAGE_H

#include "arena.h"
#include "types.h"

#define LOWPOLY_MAX_IMAGE_PIXELS ((size_t)100000000)

Image image_create_in_arena(Arena *arena, int width, int height);
int image_load_from_file(Arena *arena, const char *filename, Image *out_image);
int image_save_as_png(const Image *image, const char *filename);
int image_save_as_jpeg(const Image *image, const char *filename, int quality);
Pixel *image_pixel_at(Image *image, int x, int y);
const Pixel *image_pixel_at_const(const Image *image, int x, int y);

#endif
