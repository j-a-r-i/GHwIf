#include <gtest/gtest.h>
#include "../web.h"

TEST(Web, nasdaq)
{
    Web w;
    w.setSite(Web::NASDAQ);
    w.read();

    EXPECT_EQ ( 10.0, 10.0);
}

TEST(Web, fmi)
{
    Web w(false);
    w.setSite(Web::FMI);
    w.read();
    
    EXPECT_EQ(10.0, 10.0);
}
