#include "random.h"

void random_seed(Random *random, uint32_t seed)
{
    random->state = seed == 0 ? 0x6d2b79f5u : seed;
}

uint32_t random_next(Random *random)
{
    uint32_t value = random->state;
    value ^= value << 13;
    value ^= value >> 17;
    value ^= value << 5;
    random->state = value;
    return value;
}

int random_range(Random *random, int limit)
{
    return (int)(random_next(random) % (uint32_t)limit);
}
