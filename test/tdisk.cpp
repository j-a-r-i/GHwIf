#include <gtest/gtest.h>
#include "../disk.h"

TEST(Disk, Test1)
{
    Disk d;
    d.read();

    EXPECT_EQ ( 10.0, 10.0);
}

TEST(Disk, Test2)
{
    EXPECT_EQ(10.0, 12.0);
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
