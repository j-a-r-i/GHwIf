#include <gtest/gtest.h>
#include "../xmlparsesimple.h"

const char *STR1 = "<foo><bar one=\"1\" two=\"2\"/></foo>";

TEST(Xml, Test1)
{
    XmlParseSimple parser("bar", "one");

    parser.parse(STR1);

    EXPECT_EQ( 10.0, 10.0);
}
