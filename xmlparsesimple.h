#pragma once

#include <string>

class XmlParseSimple
{
public:
    XmlParseSimple(const char* t, const char* e) :
	tag(t), attribute(e)
    {
    }
    
    void parse(const char* str);

    bool isValidTag(const char* t);
    bool isValidAttr(const char* e);
    
private:
    std::string tag;
    std::string attribute;
};
