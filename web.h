/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#pragma once

#include <curl/curl.h>
#include "xmlparsesimple.h"

class Web
{
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
    BaseParse *parser;
    std::string url;

private:    
    CURL *handle;
    bool verbose;
};

class WebNasdaqHist : public Web
{
    WebNasdaqHist(int instrument);
};

class WebNasdaqLast : public Web
{
    WebNasdaqLast(int instrument);
};

class WebFmi : public Web
{
    WebFmi();
};

class WebStrava : public Web
{
    WebStrava();
};

class WebSunrise : public Web
{
    WebSunrise();
};

class WebCustom : public Web
{
    WebCustom(const char* site, const char* tag);
};
