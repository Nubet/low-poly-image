#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "arena.h"
#include "image.h"
#include "renderer.h"
#include "triangulation.h"

static void print_usage(const char *program)
{
    fprintf(stderr, "usage: %s input.png output.png points\n", program);
}

int main(int argc, char **argv)
{
    if (argc != 4) {
        print_usage(argv[0]);
        return 1;
    }

    int point_count = atoi(argv[3]);
    if (point_count < 4) {
        fprintf(stderr, "points must be >= 4\n");
        return 1;
    }

    srand((unsigned)time(NULL));
    Arena arena = {0};

    printf("loading %s...\n", argv[1]);
    Image input = image_load_from_file(&arena, argv[1]);
    printf("image: %dx%d\n", input.width, input.height);

    printf("generating %d points...\n", point_count);
    Point *points =
        generate_random_points_with_corners(&arena, input.width, input.height, point_count);

    printf("triangulating...\n");
    int triangle_count = 0;
    Triangle *triangles =
        build_delaunay_triangulation(&arena, points, point_count, &triangle_count);
    printf("triangles: %d\n", triangle_count);

    printf("rendering...\n");
    Image output = render_low_poly_image(&arena, &input, points, triangles, triangle_count);

    printf("saving %s...\n", argv[2]);
    image_save_as_png(&output, argv[2]);

    arena_free(&arena);
    printf("done.\n");
    return 0;
}
