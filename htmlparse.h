#pragma once

#include "xmlparsesimple.h"
#include <tidy/tidybuffio.h>

//------------------------------------------------------------------------------
class HtmlParse : public BaseParse
{
public:
    virtual ~HtmlParse() {
    }
    
    void begin();
    void parse(const char* str);
    void end();

    const char* result();

private:
    void printNode(TidyNode node, int level);
    
    TidyBuffer tbuffer;
};
