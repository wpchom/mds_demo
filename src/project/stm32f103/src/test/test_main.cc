#include "gtest/gtest.h"
#include "gmock/gmock.h"

int add(int a, int b)
{
    return (a + b);
}

TEST(AddTest, ReturnsSum) {
    EXPECT_EQ(add(1, 2), 3);
}
