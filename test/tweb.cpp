#include <gtest/gtest.h>
#include "../web.h"

TEST(Web, Test1)
{
    Web w;
    w.read();

    EXPECT_EQ ( 10.0, 10.0);
}

TEST(Web, Test2)
{
    EXPECT_EQ(10.0, 12.0);
}
