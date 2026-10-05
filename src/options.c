#include "options.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_POINT_COUNT 500

static int parse_unsigned(const char *text, unsigned int *value)
{
    char *end = NULL;
    unsigned long parsed;

    if (text[0] == '\0' || text[0] == '-')
        return 0;

    errno = 0;
    parsed = strtoul(text, &end, 10);
    if (errno == ERANGE || *end != '\0' || parsed > UINT_MAX)
        return 0;

    *value = (unsigned int)parsed;
    return 1;
}

static int parse_point_count(const char *text, int *value)
{
    unsigned int parsed;

    if (!parse_unsigned(text, &parsed) || parsed < LOWPOLY_MIN_POINT_COUNT ||
        parsed > LOWPOLY_MAX_POINT_COUNT)
        return 0;

    *value = (int)parsed;
    return 1;
}

static char *build_default_output_path(Arena *arena, const char *input_path)
{
    const char *last_slash = strrchr(input_path, '/');
    const char *last_backslash = strrchr(input_path, '\\');
    const char *filename = input_path;
    const char *extension;
    size_t stem_length;

    if (last_slash != NULL && last_slash + 1 > filename)
        filename = last_slash + 1;
    if (last_backslash != NULL && last_backslash + 1 > filename)
        filename = last_backslash + 1;

    extension = strrchr(filename, '.');
    if (extension == NULL || extension == filename)
        stem_length = strlen(input_path);
    else
        stem_length = (size_t)(extension - input_path);

    return arena_sprintf(arena, "%.*s_lowpoly.png", (int)stem_length, input_path);
}

void options_print_usage(const char *program)
{
    fprintf(stderr, "usage: %s input [--output path] [--points count] [--seed value]\n", program);
    fprintf(stderr, "defaults: points=%d, output=input_lowpoly.png, seed=random\n",
            DEFAULT_POINT_COUNT);
}

int options_parse(Arena *arena, int argc, char **argv, Options *options)
{
    options->input_path = NULL;
    options->output_path = NULL;
    options->point_count = DEFAULT_POINT_COUNT;
    options->seed = 0;
    options->seed_was_provided = 0;

    for (int i = 1; i < argc; i++) {
        const char *argument = argv[i];

        if (strcmp(argument, "--help") == 0 || strcmp(argument, "-h") == 0) {
            options_print_usage(argv[0]);
            return 0;
        }

        if (strcmp(argument, "--output") == 0) {
            if (++i >= argc) {
                fprintf(stderr, "missing value after --output\n");
                return -1;
            }
            options->output_path = argv[i];
        } else if (strcmp(argument, "--points") == 0) {
            if (++i >= argc || !parse_point_count(argv[i], &options->point_count)) {
                fprintf(stderr, "points must be an integer between %d and %d\n",
                        LOWPOLY_MIN_POINT_COUNT,
                        LOWPOLY_MAX_POINT_COUNT);
                return -1;
            }
        } else if (strcmp(argument, "--seed") == 0) {
            if (++i >= argc || !parse_unsigned(argv[i], &options->seed)) {
                fprintf(stderr, "seed must be an unsigned integer\n");
                return -1;
            }
            options->seed_was_provided = 1;
        } else if (argument[0] == '-') {
            fprintf(stderr, "unknown option: %s\n", argument);
            return -1;
        } else if (options->input_path == NULL) {
            options->input_path = argument;
        } else {
            fprintf(stderr, "unexpected argument: %s\n", argument);
            return -1;
        }
    }

    if (options->input_path == NULL) {
        options_print_usage(argv[0]);
        return -1;
    }

    if (options->output_path == NULL)
        options->output_path = build_default_output_path(arena, options->input_path);

    return 1;
}
