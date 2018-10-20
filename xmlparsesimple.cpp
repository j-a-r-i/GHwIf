#include <iostream>
#include <expat.h>
#include <string.h>
#include "xmlparsesimple.h"

#define VERBOSE

void onStartElem(void *data, const char* element, const char** attributes)
{
    XmlParseSimple *self = (XmlParseSimple*)data;

#ifdef VERBOSE	
	std::cout << "+" << element << std::endl;
#endif

    if (self->isValidTag(element))
    {

	for (int i=0; attributes[i]; i+=2) {
	    if (self->isValidAttr(attributes[i]))
		std::cout << "\t" << attributes[i] << "=" << attributes[i+1] << std::endl;
	}
    }
}

void onEndElem(void *data, const char* element)
{
#ifdef VERBOSE
    //std::cout << "-" << element << std::endl;
#endif
}

bool XmlParseSimple::isValidTag(const char* t)
{
    return (tag == t);
}

bool XmlParseSimple::isValidAttr(const char* e)
{
    return (attribute == e);
}


void XmlParseSimple::begin()
{
    parser = XML_ParserCreate(NULL);

    XML_SetUserData(parser, this);
    XML_SetElementHandler(parser, onStartElem, onEndElem);
}

void XmlParseSimple::parse(const char* str)
{
    if (XML_Parse(parser, str, strlen(str), XML_FALSE) == XML_STATUS_ERROR) {
	std::cout << "XML parsing error: "
		  << XML_ErrorString(XML_GetErrorCode(parser))
		  << std::endl;
    }
}

void XmlParseSimple::end()
{
#ifdef VERBOSE
    std::cout << "XML parse end" << std::endl;
#endif
    XML_ParserFree(parser);
}
