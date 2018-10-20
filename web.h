/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#pragma once

#include <curl/curl.h>
#include "infoitem.h"
#include "xmlparsesimple.h"

class Web : public InfoItem
{
public:
    enum Site {
	NASDAQ,
	FMI
    };
    
    Web(bool verbose = false);

    virtual ~Web();

    void setSite(Site s) {
	site = s;
    }
    
    void read();
    void print();

    void onData(const char* str);

private:
    Site site;
    CURL *handle;
    bool verbose;
    XmlBase *parser;
};
