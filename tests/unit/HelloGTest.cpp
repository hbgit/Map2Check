#include <gtest/gtest.h>

int add_test(int a, int b) { return a + b; }

TEST(testHello, myAddTest) { EXPECT_EQ(10, add_test(4, 6)); }
