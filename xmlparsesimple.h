#pragma once

#include <string>
#include <expat.h>

class XmlBase
{
public:
    virtual void onStartElem(const char* element, const char** attributes) = 0;
    virtual void onEndElem(const char* element) = 0;
    virtual void onData(const char* data, int len) = 0;

    void begin();
    void parse(const char* str);
    void end();

private:
    XML_Parser parser;    
};

class XmlParseTag : public XmlBase
{
public:
    XmlParseTag(const char* t, const char* e) :
	tag(t), attribute(e)
    {
    }

    std::string& getLast() {
	return lastValue;
    }

    void onStartElem(const char* element, const char** attributes);
    void onEndElem(const char* element);
    void onData(const char* data, int len);
    
private:
    std::string tag;
    std::string attribute;

    std::string lastValue;
};
