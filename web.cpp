/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#include "web.h"
#include "config.h"
#include "xmlparsesimple.h"
#include <iostream>
#include <sstream>

#define SITE_NASDAQ "www.nasdaqomxnordic.com"
#define SITE_FMI    "data.fmi.fi"
#define SITE_STRAVA "www.strava.com"
#define SITE_DWEET  "dweet.io"

size_t curl_write(void *buffer, size_t size, size_t nmemb, void *user_data)
{
    Web *self = (Web*)user_data;

    if (self != NULL)
	self->onData((const char*)buffer);

    return size * nmemb;
}

Web::Web(bool v)
{
    curl_global_init(CURL_GLOBAL_ALL);

    handle = curl_easy_init();

    verbose = v;
}

Web::~Web()
{
    if (handle != NULL)
	curl_easy_cleanup(handle);
    
    curl_global_cleanup();
}

#define SEP "&"

void Web::read()
{
    if (handle == NULL) {
	std::cout << "CURL not initialized!" << std::endl;
	return;
    }

    CURLcode result;
    std::ostringstream os;

    if (site == NASDAQ) {
	os << "https://" << SITE_NASDAQ
	   << "/webproxy/DataFeedProxy.aspx?"
	   << "Subsystem="     << "History" << SEP
	   << "Action=Get"     << "DataSeries" << SEP
	   << "Instrument=HEX" << "24311" << SEP
	   << "FromDate="      << "2018-09-24";
    }

    else if (site == FMI) {
	os << "http://" << SITE_FMI
	   << "/fmi-apikey/"   << FMI_API << "/wfs?"
	   << "request="        << "getFeature" << SEP
	   << "storedquery_id=" << "fmi::forecast::hirlam::surface::point::multipointcoverage" << SEP
	   << "place="          << "oittaa" << SEP
	   << "parameters="     << "temperature,dewpoint,windspeedms,precipitation1h";
    }
    
#if 0

    os << "https://" << SITE_STRAVA 
       << "/api/v3/athlete/activities?"
       << "page="   << 1 << SEP
       << "access_token=" << strava_api << SEP;

    os << "https://" << SITE_DWEET
       << "/dweet/for/ha.joj.home?"
       << "foo=" << 1 << SEP
       << "bar=" << 2 << SEP;
#endif

    curl_easy_setopt(handle, CURLOPT_URL, os.str().c_str()); //"http://www.iltalehti.fi/index.html");
    curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);

    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, curl_write);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, this);


    parser = new XmlParseTag("hi", "cp");

    parser->begin();
    result = curl_easy_perform(handle);
    if (result != CURLE_OK) {
	std::cout << "CURL ERROR: " << curl_easy_strerror(result) << std::endl;
    }

    parser->end();
    delete parser;
    parser = NULL;
}

void Web::onData(const char* str)
{
    if (parser != NULL)
	parser->parse(str);

    if (verbose)
	std::cout << "CURL: " << (char*)str << std::endl;
}

void Web::print()
{
}
