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

    void setSite(Site s, int arg) {
	site = s;
	siteArg = arg;
    }

    void setVerbose(bool val) {
	verbose = val;
    }
    
    void read();
    void print();

    void onData(const char* str);

private:
    Site site;
    
    /** Parameter to create site name. This is site dependent.
     */
    uint siteArg;
    
    CURL *handle;
    bool verbose;
    XmlBase *parser;
};
