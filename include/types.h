#ifndef LOWPOLY_TYPES_H
#define LOWPOLY_TYPES_H

#include <stdint.h>

typedef struct {
    uint8_t r, g, b;
} Pixel;

_Static_assert(sizeof(Pixel) == 3, "Pixel must contain packed RGB data");

typedef struct {
    int width;
    int height;
    Pixel *pixels;
} Image;

typedef struct {
    double x;
    double y;
} Point;

typedef struct {
    int a;
    int b;
    int c;
} Triangle;

typedef struct {
    int a;
    int b;
} Edge;

#endif
