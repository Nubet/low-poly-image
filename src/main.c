#include <stdio.h>
#include <time.h>

#include "arena.h"
#include "image.h"
#include "options.h"
#include "processing.h"

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

    printf("seed: %u\n", options.seed);
    LowpolyProcessRequest request = {
        .input_path = options.input_path,
        .point_count = options.point_count,
        .seed = options.seed,
    };
    LowpolyProcessResult result = {0};
    if (lowpoly_process(&arena, &request, &result) != LOWPOLY_PROCESS_SUCCESS) {
        arena_free(&arena);
        return 1;
    }
    printf("saving %s...\n", options.output_path);
    if (!image_save_as_png(&result.output, options.output_path)) {
        arena_free(&arena);
        return 1;
    }

    arena_free(&arena);
    printf("done.\n");
    return 0;
}
