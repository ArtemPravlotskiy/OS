//
// Created by pravl on 12.04.2025.
//

#include <gtest/gtest.h>
#include "../src/processArray.cpp"
#include <vector>

TEST(ProcessArrayTest, ReplaceMinMaxWithAverage) {
    std::vector<int> input = {3, 5, 7, 9, 11};
    // (3+5+7+9+11)/5 = 7.
    std::vector<int> expected = {7, 5, 7, 9, 7};

    std::vector<int> result = processArray(input);
    EXPECT_EQ(result, expected);
}

TEST(ProcessArrayTest, ThrowsOnEmptyArray) {
    std::vector<int> empty;
    EXPECT_THROW(processArray(empty), std::invalid_argument);
}

TEST(ProcessArrayTest, MultipleMinMaxOccurrences) {
    std::vector<int> input = {4, 2, 2, 8, 8, 6};
    // (4+2+2+8+8+6)/6 = 30/6 = 5.
    std::vector<int> expected = {4, 5, 5, 5, 5, 6};
    std::vector<int> result = processArray(input);
    EXPECT_EQ(result, expected);
}
