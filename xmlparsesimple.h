#pragma once

#include <string>
#include <expat.h>

//------------------------------------------------------------------------------
class BaseParse
{
public:
    virtual ~BaseParse() {
    }
    
    virtual void begin() = 0;
    virtual void parse(const char* str) = 0;
    virtual void end() = 0;

    virtual const char* result() = 0;
};

//------------------------------------------------------------------------------
class DummyParse : public BaseParse
{
public:
    void begin();
    void parse(const char* str);
    void end();

    const char* result();
};


//------------------------------------------------------------------------------
class XmlBase : public BaseParse
{
public:
    virtual void onStartElem(const char* element, const char** attributes) = 0;
    virtual void onEndElem(const char* element) = 0;
    virtual void onData(const char* data, int len) = 0;

    void begin();
    void parse(const char* str);
    void end();

    const char* result() {
	return lastValue.c_str();
    }
    
protected:
    std::string lastValue;

private:
    XML_Parser parser;    
};

//------------------------------------------------------------------------------
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
};

//------------------------------------------------------------------------------
class XmlParseData : public XmlBase
{
public:
    XmlParseData(const char* t) :
	tag(t),
	valid(false)
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
    bool valid;
};
