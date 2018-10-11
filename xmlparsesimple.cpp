#include <iostream>
#include <expat.h>
#include <string.h>
#include "xmlparsesimple.h"

void onStartElem(void *data, const char* element, const char** attributes)
{
    XmlParseSimple *self = (XmlParseSimple*)data;

    if (self->isValidTag(element))
    {
	//std::cout << "+" << element << std::endl;

	for (int i=0; attributes[i]; i+=2) {
	    if (self->isValidAttr(attributes[i]))
		std::cout << "\t" << attributes[i] << "=" << attributes[i+1] << std::endl;
	}
    }
}

void onEndElem(void *data, const char* element)
{
    //std::cout << "-" << element << std::endl;
}

bool XmlParseSimple::isValidTag(const char* t)
{
    return (tag == t);
}

bool XmlParseSimple::isValidAttr(const char* e)
{
    return (attribute == e);
}

void XmlParseSimple::parse(const char* str)
{
    XML_Parser parser = XML_ParserCreate(NULL);

    XML_SetUserData(parser, this);
    XML_SetElementHandler(parser, onStartElem, onEndElem);

    if (XML_Parse(parser, str, strlen(str), XML_TRUE) == XML_STATUS_ERROR) {
	std::cout << "XML parsing error "
		  << XML_ErrorString(XML_GetErrorCode(parser))
		  << std::endl;
    }

    XML_ParserFree(parser);
}
