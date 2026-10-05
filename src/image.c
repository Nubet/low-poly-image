#include "image.h"

#include <stdio.h>
#include <string.h>

#include "stb_image.h"
#include "stb_image_write.h"

Image image_create_in_arena(Arena *arena, int width, int height)
{
    size_t pixel_count = (size_t)width * (size_t)height;
    Image image = {.width = width,
                   .height = height,
                   .pixels = arena_alloc(arena, pixel_count * sizeof(Pixel))};

    memset(image.pixels, 0, pixel_count * sizeof(Pixel));
    return image;
}

Pixel *image_pixel_at(Image *image, int x, int y)
{
    return &image->pixels[y * image->width + x];
}

const Pixel *image_pixel_at_const(const Image *image, int x, int y)
{
    return &image->pixels[y * image->width + x];
}

int image_load_from_file(Arena *arena, const char *filename, Image *out_image)
{
    int width;
    int height;
    int channels;

    *out_image = (Image){0};

    if (!stbi_info(filename, &width, &height, &channels)) {
        fprintf(stderr, "could not inspect %s: %s\n", filename, stbi_failure_reason());
        return 0;
    }

    if (width <= 0 || height <= 0 || (size_t)width > LOWPOLY_MAX_IMAGE_PIXELS / (size_t)height) {
        fprintf(stderr, "image %s is too large\n", filename);
        return 0;
    }

    unsigned char *source = stbi_load(filename, &width, &height, NULL, 3);

    if (!source) {
        fprintf(stderr, "could not load %s: %s\n", filename, stbi_failure_reason());
        return 0;
    }

    *out_image = image_create_in_arena(arena, width, height);
    size_t pixel_count = (size_t)width * (size_t)height;

    for (size_t i = 0; i < pixel_count; i++) {
        out_image->pixels[i].r = source[i * 3];
        out_image->pixels[i].g = source[i * 3 + 1];
        out_image->pixels[i].b = source[i * 3 + 2];
    }

    stbi_image_free(source);
    (void)channels;
    return 1;
}

int image_save_as_png(const Image *image, const char *filename)
{
    int stride = image->width * (int)sizeof(Pixel);

    if (!stbi_write_png(filename, image->width, image->height, 3, image->pixels, stride)) {
        fprintf(stderr, "could not save %s\n", filename);
        return 0;
    }

    return 1;
}
