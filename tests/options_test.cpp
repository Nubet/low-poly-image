#include <gtest/gtest.h>

extern "C" {
#include "arena.h"
#include "options.h"
}

TEST(Options, UsesDefaultsForInputOnly)
{
    char input[] = "photos/input.jpg";
    char *arguments[] = {const_cast<char *>("lowpoly"), input};
    Arena arena = {};
    Options options = {};

    ASSERT_EQ(options_parse(&arena, 2, arguments, &options), 1);
    EXPECT_STREQ(options.input_path, "photos/input.jpg");
    EXPECT_STREQ(options.output_path, "photos/input_lowpoly.png");
    EXPECT_EQ(options.point_count, 500);
    EXPECT_EQ(options.seed_was_provided, 0);

    arena_free(&arena);
}

TEST(Options, ParsesCustomValues)
{
    char input[] = "input.png";
    char output[] = "result.png";
    char points[] = "42";
    char seed[] = "123";
    char *arguments[] = {
        const_cast<char *>("lowpoly"), input, const_cast<char *>("--points"), points,
        const_cast<char *>("--seed"),  seed,  const_cast<char *>("--output"), output};
    Arena arena = {};
    Options options = {};

    ASSERT_EQ(options_parse(&arena, 8, arguments, &options), 1);
    EXPECT_EQ(options.point_count, 42);
    EXPECT_EQ(options.seed, 123u);
    EXPECT_EQ(options.seed_was_provided, 1);
    EXPECT_STREQ(options.output_path, "result.png");

    arena_free(&arena);
}

TEST(Options, RejectsInvalidPointCount)
{
    char input[] = "input.png";
    char points[] = "not-a-number";
    char *arguments[] = {const_cast<char *>("lowpoly"), input, const_cast<char *>("--points"),
                         points};
    Arena arena = {};
    Options options = {};

    EXPECT_EQ(options_parse(&arena, 4, arguments, &options), -1);
    arena_free(&arena);
}

TEST(Options, RejectsUnknownOption)
{
    char input[] = "input.png";
    char *arguments[] = {const_cast<char *>("lowpoly"), input, const_cast<char *>("--unknown")};
    Arena arena = {};
    Options options = {};

    EXPECT_EQ(options_parse(&arena, 3, arguments, &options), -1);
    arena_free(&arena);
}

TEST(Options, RejectsMissingInput)
{
    char *arguments[] = {const_cast<char *>("lowpoly")};
    Arena arena = {};
    Options options = {};

    EXPECT_EQ(options_parse(&arena, 1, arguments, &options), -1);
    arena_free(&arena);
}

TEST(Options, AcceptsHelpWithoutInput)
{
    char *arguments[] = {const_cast<char *>("lowpoly"), const_cast<char *>("--help")};
    Arena arena = {};
    Options options = {};

    EXPECT_EQ(options_parse(&arena, 2, arguments, &options), 0);
    arena_free(&arena);
}

TEST(Options, RejectsMissingOptionValues)
{
    const char *option_names[] = {"--output", "--points", "--seed"};

    for (const char *option_name : option_names) {
        char input[] = "input.png";
        char *arguments[] = {const_cast<char *>("lowpoly"), input, const_cast<char *>(option_name)};
        Arena arena = {};
        Options options = {};

        EXPECT_EQ(options_parse(&arena, 3, arguments, &options), -1);
        arena_free(&arena);
    }
}

TEST(Options, RejectsNegativeAndOverflowingValues)
{
    char input[] = "input.png";
    char negative[] = "-1";
    char overflowing[] = "999999999999999999999999999999";

    char *negative_arguments[] = {const_cast<char *>("lowpoly"), input,
                                  const_cast<char *>("--points"), negative};
    char *overflowing_arguments[] = {const_cast<char *>("lowpoly"), input,
                                     const_cast<char *>("--seed"), overflowing};
    Arena arena = {};
    Options options = {};

    EXPECT_EQ(options_parse(&arena, 4, negative_arguments, &options), -1);
    EXPECT_EQ(options_parse(&arena, 4, overflowing_arguments, &options), -1);
    arena_free(&arena);
}
