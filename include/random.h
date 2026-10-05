#ifndef LOWPOLY_RANDOM_H
#define LOWPOLY_RANDOM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t state;
} Random;

void random_seed(Random *random, uint32_t seed);
uint32_t random_next(Random *random);
int random_range(Random *random, int limit);

#ifdef __cplusplus
}
#endif

#endif
