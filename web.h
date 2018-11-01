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
	NASDAQ_HIST,
	NASDAQ_LAST,
	FMI,
	STRAVA
    };
    
    Web(bool verbose = false);

    virtual ~Web();

    void setSite(Site s) {
	site = s;
    }

    void setVerbose(bool val) {
	verbose = val;
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
