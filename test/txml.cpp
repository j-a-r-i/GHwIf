#include <gtest/gtest.h>
#include "../xmlparsesimple.h"

const char *STR1 = "<foo><bar one=\"1\" two=\"2\"/></foo>";

void load_parser(XmlParseTag *parser)
{
    parser->begin();
    parser->parse(STR1);
    parser->end();    
}

TEST(Xml, test1)
{
    XmlParseTag parser("bar", "one");

    load_parser(&parser);

    EXPECT_EQ( parser.getLast(), "1");
}

TEST(Xml, test2)
{
    XmlParseTag parser("bar", "two");

    load_parser(&parser);

    EXPECT_EQ( parser.getLast(), "21");
}


TEST(Xml, test3)
{
    XmlParseTag parser("bar", "not_found");

    load_parser(&parser);

    EXPECT_EQ( parser.getLast(), "");
}
