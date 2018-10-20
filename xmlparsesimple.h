#pragma once

#include <string>
#include <expat.h>

class XmlParseSimple
{
public:
    XmlParseSimple(const char* t, const char* e) :
	tag(t), attribute(e)
    {
    }

    void begin();
    void parse(const char* str);
    void end();
    
    bool isValidTag(const char* t);
    bool isValidAttr(const char* e);
    
private:
    std::string tag;
    std::string attribute;

    XML_Parser parser;
};
