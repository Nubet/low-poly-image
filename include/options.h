#ifndef LOWPOLY_OPTIONS_H
#define LOWPOLY_OPTIONS_H

#include "arena.h"
#include "lowpoly_limits.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *input_path;
    const char *output_path;
    int point_count;
    unsigned int seed;
    int seed_was_provided;
} Options;

// Returns 1 for success, 0 when help was printed, and -1 for invalid arguments
int options_parse(Arena *arena, int argc, char **argv, Options *options);
void options_print_usage(const char *program);

#ifdef __cplusplus
}
#endif

#endif
