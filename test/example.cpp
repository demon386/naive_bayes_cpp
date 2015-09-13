#include <gtest/gtest.h>
#include <vector>
#include "example.hpp"

TEST(example, dummy) {
    std::vector<std::string> words = {"hello", "world"};
    auto label = "positive";
    Example example(words, label);

    EXPECT_EQ("positive", example._label);
}