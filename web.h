/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#pragma once

#include <curl/curl.h>
#include "infoitem.h"
#include "xmlparsesimple.h"

class Web
{
public:
    enum Site {
	NASDAQ_HIST,
	NASDAQ_LAST,
	FMI,
	STRAVA,
	SUNRISE,
	CUSTOM
    };
    
    Web(bool verbose = false);

    virtual ~Web();

    void setSite(Site s, int arg);
    void setSite(const char* site, const char *tag);
    
    void setVerbose(bool val) {
	verbose = val;
    }
    
    void operator()();

    void onData(const char* str);

private:    
    CURL *handle;
    bool verbose;
    BaseParse *parser;
    std::string url;
};
