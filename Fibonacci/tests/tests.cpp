//
// Created by pravl on 23.02.2025.
//

#include <gtest/gtest.h>
#include "fibonacci.h"

class FibonacciTests : public testing::Test {
protected:
    Fibonacci fib;
};

TEST_F(FibonacciTests, GetSequenceValidInput) {
    // n = 10
    const std::vector<unsigned long> expected = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};

    const std::vector<unsigned long> actual = fib.getSequence(10);
    ASSERT_EQ(expected.size(), actual.size());
    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_EQ(expected[i], actual[i]);
    }
}

TEST_F(FibonacciTests, InputThrowsException) {
    EXPECT_THROW(fib.getSequence(-5), std::invalid_argument);
    EXPECT_THROW(fib.getSequence(-1), std::invalid_argument);
    EXPECT_THROW(fib.getSequence(0), std::invalid_argument);
    EXPECT_NO_THROW(fib.getSequence(1));
    EXPECT_NO_THROW(fib.getSequence(5));
}