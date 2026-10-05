#include <gtest/gtest.h>

extern "C" {
#include "random.h"
}

TEST(Random, SameSeedProducesSameSequence)
{
    Random first = {};
    Random second = {};
    random_seed(&first, 123);
    random_seed(&second, 123);

    for (int i = 0; i < 20; ++i)
        EXPECT_EQ(random_next(&first), random_next(&second));
}

TEST(Random, RangeStaysWithinLimit)
{
    Random random = {};
    random_seed(&random, 123);

    for (int i = 0; i < 1000; ++i) {
        int value = random_range(&random, 17);
        EXPECT_GE(value, 0);
        EXPECT_LT(value, 17);
    }
}
