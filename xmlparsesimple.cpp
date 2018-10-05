#include <iostream>
#include <expat.h>
#include <string.h>
#include "xmlparsesimple.h"

void onStartElem(void *data, const char* element, const char** attributes)
{
    std::cout << "+" << element << std::endl;
}

void onEndElem(void *data, const char* element)
{
    std::cout << "-" << element << std::endl;
}

void XmlParseSimple::parse(const char* str)
{
    XML_Parser parser = XML_ParserCreate(NULL);

    XML_SetElementHandler(parser, onStartElem, onEndElem);

    if (XML_Parse(parser, str, strlen(str), XML_TRUE) == XML_STATUS_ERROR) {
	std::cout << "XML parsing error "
		  << XML_ErrorString(XML_GetErrorCode(parser))
		  << std::endl;
    }

    XML_ParserFree(parser);
}
