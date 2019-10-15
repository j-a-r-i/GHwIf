/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#pragma once

#include <curl/curl.h>
#include "xmlparsesimple.h"

class Web {
public:   
    Web(bool verbose = false);

    virtual ~Web();
    
    void setVerbose(bool val) {
	verbose = val;
    }
    
    void operator()();

    void onData(const char* str);

    /** prints curl version used.
     */
    void version();

protected:
	BaseParse* parser;
	std::string url;

private:    
    CURL *handle;
    bool verbose;
};

class WebFmi : public Web {
public:
	WebFmi();
};

class WebSunrise : public Web {
public:
	WebSunrise();
};

class WebStrava : public Web {
public:
	WebStrava();
};

class WebNasdaqHist : public Web {
public:
	static const int INST_NOKIA = 24311;

	WebNasdaqHist(int instrument);
};

class WebNasdaqLast : public Web {
public:
	WebNasdaqLast(int instrument);
};