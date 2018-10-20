#include <iostream>
#include <expat.h>
#include <string.h>
#include "xmlparsesimple.h"

//#define VERBOSE

void onXmlStartElem(void *data, const char* element, const char** attributes)
{
    XmlBase *self = (XmlBase*)data;

#ifdef VERBOSE	
    std::cout << "+" << element << std::endl;
#endif

    if (self != NULL)
	self->onStartElem(element, attributes);
}

void onXmlEndElem(void *data, const char* element)
{
    XmlBase *self = (XmlBase*)data;
#ifdef VERBOSE
    //std::cout << "-" << element << std::endl;
#endif

    if (self != NULL)
	self->onEndElem(element);
}

void onXmlData(void *data, const char* str, int len)
{
    XmlBase *self = (XmlBase*)data;
#ifdef VERBOSE
//    std::cout << "DATA:" << str << std::endl;
#endif

    if (self != NULL) {
	//str[len] = 0;
	self->onData(str, len);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlBase::begin()
{
    parser = XML_ParserCreate(NULL);

    XML_SetUserData(parser, this);
    XML_SetElementHandler(parser, onXmlStartElem, onXmlEndElem);
    XML_SetCharacterDataHandler(parser, onXmlData);
}

void XmlBase::parse(const char* str)
{
    if (XML_Parse(parser, str, strlen(str), XML_FALSE) == XML_STATUS_ERROR) {
	std::cout << "XML parsing error: "
		  << XML_ErrorString(XML_GetErrorCode(parser))
		  << std::endl;
    }
}

void XmlBase::end()
{
#ifdef VERBOSE
    std::cout << "XML parse end" << std::endl;
#endif
    XML_ParserFree(parser);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void XmlParseTag::onStartElem(const char* element, const char** attributes)
{
    if (tag == element) {
	for (int i=0; attributes[i]; i+=2) {
	    if (attribute == attributes[i]) {
		lastValue = attributes[i+1];
		//std::cout << "\t" << attribute << "=" << lastValue << std::endl;
	    }
	}
    }
}

void XmlParseTag::onEndElem(const char* element)
{
    // end tag is not handled
}

void XmlParseTag::onData(const char* data, int len)
{
    if ((len == 1) && (data[0] == 10))
	return;
    if (len == 1) {
	//std::cout << "D:" << (int)data[0] << std::endl;
	return;
    }
    
    std::string d(data, len);

    //std::cout << "D:" << d << "(len=" << len << ")" << std::endl;
}
