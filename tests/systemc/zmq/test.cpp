#include <gtest/gtest.h>

bool f() {
    return true;
}

TEST(example, lol) {
    EXPECT_EQ(true, true);
    const bool result = f();
    EXPECT_EQ(true, result);
}
