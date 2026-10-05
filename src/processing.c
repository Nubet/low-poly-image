#include "processing.h"

#include "image.h"
#include "random.h"
#include "renderer.h"
#include "triangulation.h"

LowpolyProcessStatus lowpoly_process(Arena *arena, const LowpolyProcessRequest *request,
                                      LowpolyProcessResult *result)
{
    if (arena == NULL || request == NULL || result == NULL || request->input_path == NULL ||
        request->input_path[0] == '\0' || request->point_count < LOWPOLY_MIN_POINT_COUNT ||
        request->point_count > LOWPOLY_MAX_POINT_COUNT) {
        return LOWPOLY_PROCESS_INVALID_REQUEST;
    }

    *result = (LowpolyProcessResult){0};

    Image input = {0};
    if (!image_load_from_file(arena, request->input_path, &input))
        return LOWPOLY_PROCESS_INPUT_ERROR;

    Random random = {0};
    random_seed(&random, request->seed);

    Point *points = generate_random_points_with_corners(arena, input.width, input.height,
                                                        request->point_count, &random);
    if (points == NULL)
        return LOWPOLY_PROCESS_TRIANGULATION_ERROR;

    int triangle_count = 0;
    Triangle *triangles =
        build_delaunay_triangulation(arena, points, request->point_count, &triangle_count);
    if (triangles == NULL)
        return LOWPOLY_PROCESS_TRIANGULATION_ERROR;

    result->output = render_low_poly_image(arena, &input, points, triangles, triangle_count);
    result->triangle_count = triangle_count;
    return LOWPOLY_PROCESS_SUCCESS;
}
