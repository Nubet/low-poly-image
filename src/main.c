#include <stdio.h>
#include <time.h>

#include "arena.h"
#include "image.h"
#include "options.h"
#include "random.h"
#include "renderer.h"
#include "triangulation.h"

static double elapsed_milliseconds(clock_t started)
{
    return (double)(clock() - started) * 1000.0 / (double)CLOCKS_PER_SEC;
}

int main(int argc, char **argv)
{
    Arena arena = {0};
    Options options = {0};

    int options_result = options_parse(&arena, argc, argv, &options);
    if (options_result <= 0) {
        arena_free(&arena);
        return options_result == 0 ? 0 : 1;
    }

    if (!options.seed_was_provided)
        options.seed = (unsigned int)time(NULL) ^ (unsigned int)clock();

    Random random = {0};
    random_seed(&random, options.seed);

    printf("seed: %u\n", options.seed);
    clock_t started = clock();
    printf("loading %s...\n", options.input_path);
    Image input = {0};
    if (!image_load_from_file(&arena, options.input_path, &input)) {
        arena_free(&arena);
        return 1;
    }
    printf("image: %dx%d (%.0f ms)\n", input.width, input.height, elapsed_milliseconds(started));

    started = clock();
    printf("generating %d points...\n", options.point_count);
    Point *points = generate_random_points_with_corners(&arena, input.width, input.height,
                                                        options.point_count, &random);

    printf("generating done (%.0f ms)\n", elapsed_milliseconds(started));

    started = clock();
    printf("triangulating...\n");
    int triangle_count = 0;
    Triangle *triangles =
        build_delaunay_triangulation(&arena, points, options.point_count, &triangle_count);
    if (triangles == NULL) {
        arena_free(&arena);
        return 1;
    }
    printf("triangles: %d (%.0f ms)\n", triangle_count, elapsed_milliseconds(started));

    started = clock();
    printf("rendering...\n");
    Image output = render_low_poly_image(&arena, &input, points, triangles, triangle_count);

    printf("rendering done (%.0f ms)\n", elapsed_milliseconds(started));

    started = clock();
    printf("saving %s...\n", options.output_path);
    if (!image_save_as_png(&output, options.output_path)) {
        arena_free(&arena);
        return 1;
    }
    printf("saving done (%.0f ms)\n", elapsed_milliseconds(started));

    arena_free(&arena);
    printf("done.\n");
    return 0;
}
