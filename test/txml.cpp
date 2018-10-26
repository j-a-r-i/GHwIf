#include <gtest/gtest.h>
#include "../xmlparsesimple.h"

const char *STR1 = R"(
<foo>
  <bar one="1" two="2"/>
</foo>)";

const char *STR1b = R"(
<foo>
  <bar one="1" two="2"/>
  <bar one="a"/>
</foo>)";

const char *STR2 = R"(
<foo>
  <one>test1</one>
  <two>test2</two>
</foo>)";


void load_parser(XmlBase *parser, const char* data)
{
    parser->begin();
    parser->parse(data);
    parser->end();    
}

TEST(xml, tag1)
{
    XmlParseTag parser("bar", "one");

    load_parser(&parser, STR1);

    EXPECT_EQ( parser.getLast(), "1");
}

TEST(xml, tag2)
{
    XmlParseTag parser("bar", "two");

    load_parser(&parser, STR1);

    EXPECT_EQ( parser.getLast(), "2");
}

TEST(xml, tag3)
{
    XmlParseTag parser("bar", "one");

    load_parser(&parser, STR1b);

    EXPECT_EQ( parser.getLast(), "a");
}

TEST(xml, tag4)
{
    XmlParseTag parser("bar", "not_found");

    load_parser(&parser, STR1);

    EXPECT_EQ( parser.getLast(), "");
}


TEST(xml, data1)
{
    XmlParseData parser("one");

    load_parser(&parser, STR2);

    EXPECT_EQ( parser.getLast(), "test1");
}

TEST(xml, data2)
{
    XmlParseData parser("two");

    load_parser(&parser, STR2);

    EXPECT_EQ( parser.getLast(), "test2");
}

