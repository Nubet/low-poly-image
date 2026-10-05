#ifndef LOWPOLY_PROCESSING_H
#define LOWPOLY_PROCESSING_H

#include <stdint.h>

#include "arena.h"
#include "lowpoly_limits.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *input_path;
    int point_count;
    uint32_t seed;
} LowpolyProcessRequest;

typedef struct {
    Image output;
    int triangle_count;
} LowpolyProcessResult;

typedef enum {
    LOWPOLY_PROCESS_SUCCESS = 0,
    LOWPOLY_PROCESS_INVALID_REQUEST,
    LOWPOLY_PROCESS_INPUT_ERROR,
    LOWPOLY_PROCESS_TRIANGULATION_ERROR,
} LowpolyProcessStatus;

LowpolyProcessStatus lowpoly_process(Arena *arena, const LowpolyProcessRequest *request,
                                      LowpolyProcessResult *result);

#ifdef __cplusplus
}
#endif

#endif
