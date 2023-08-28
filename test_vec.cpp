#include <gtest/gtest.h>
#include "include/vec.hpp"

TEST(VecAdditionTest, AddVecsOfSameSize) {
    Vec v1({1.0, 2.0, 3.0});
    Vec v2({4.0, 5.0, 6.0});
    Vec result = v1 + v2;

    ASSERT_EQ(result.values.size(), 3);
    EXPECT_DOUBLE_EQ(result.values[0], 5.0);
    EXPECT_DOUBLE_EQ(result.values[1], 7.0);
    EXPECT_DOUBLE_EQ(result.values[2], 9.0);
}

TEST(VecAdditionTest, AddVecsOfDifferentSize) {
    Vec v1({1.0, 2.0});
    Vec v2({4.0, 5.0, 6.0});

    EXPECT_THROW(v1 + v2, std::invalid_argument);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
