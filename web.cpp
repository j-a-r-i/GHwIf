/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#include "web.h"
#include "config.h"
#include "xmlparsesimple.h"
#include "htmlparse.h"
#include "logger.h"
#include <sstream>

#define SITE_NASDAQ  "www.nasdaqomxnordic.com"
#define SITE_FMI     "data.fmi.fi"
#define SITE_STRAVA  "www.strava.com"
#define SITE_SUNRISE "api.sunrise-sunset.org"
#define SITE_DWEET   "dweet.io"

size_t curl_write(void *buffer, size_t size, size_t nmemb, void *user_data)
{
    Web *self = (Web*)user_data;

    if (self != NULL)
	self->onData((const char*)buffer);

    return size * nmemb;
}

Web::Web(bool v) : InfoReader("web")
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

void Web::setSite(Site site, int siteArg)
{
    std::ostringstream os;

    if (site == NASDAQ_HIST) {
	os << "https://" << SITE_NASDAQ
	   << "/webproxy/DataFeedProxy.aspx?"
	   << "Subsystem="     << "History" << SEP
	   << "Action=Get"     << "DataSeries" << SEP
	   << "Instrument=HEX" << siteArg << SEP
	   << "FromDate="      << "2018-09-24";

	parser = new XmlParseTag("hi", "cp");
    }
//SiteArg 24311 = NOKIA
    
    else if (site == NASDAQ_LAST) {
	os << "https://" << SITE_NASDAQ
	   << "/webproxy/DataFeedProxy.aspx?"
	   << "Subsystem="     << "Prices" << SEP
	   << "Action=Get"     << "Instrument" << SEP
	   << "Instrument=HEX" << siteArg;

	parser = new XmlParseTag("inst", "lsp");
    }

    else if (site == FMI) {
	os << "http://" << SITE_FMI
	   << "/fmi-apikey/"   << FMI_API << "/wfs?"
	   << "request="        << "getFeature" << SEP
	   << "storedquery_id=" << "fmi::forecast::hirlam::surface::point::multipointcoverage" << SEP
	   << "place="          << "oittaa" << SEP
	   << "parameters="     << "temperature,dewpoint,windspeedms,precipitation1h";

	parser = new XmlParseTag("hi", "cp");
    }

    else if (site == STRAVA) {
	os << "https://" << SITE_STRAVA 
	   << "/api/v3/athlete/activities?"
	   << "page="   << 1 << SEP
	   << "access_token=" << STRAVA_API << SEP;

	parser = new DummyParse();
    }

    else if (site == SUNRISE) {
	os << "https://" << SITE_SUNRISE
	   << "/json?"
	   << "lat=" << Cfg::get(CFG_LOCATION_LAT) << SEP
	   << "lng=" << Cfg::get(CFG_LOCATION_LON);
	    //<< "date=" << "today";

	parser = new DummyParse();
    }

    else {
	Log::err("web", "invalid site");
	url = "http://none";
	return;
    }
    
#if 0
    os << "https://" << SITE_DWEET
       << "/dweet/for/ha.joj.home?"
       << "foo=" << 1 << SEP
       << "bar=" << 2 << SEP;
#endif

    url = os.str();
}

void Web::setSite(const char* site, const char *tag)
{
    url = site;
    parser = new HtmlParse();
}


void Web::read()
{
    if (handle == NULL) {
	Log::err("Web::read", "CURL not initialized");
	return;
    }

    CURLcode result;

    Log::msg("web", url.c_str());
    
    curl_easy_setopt(handle, CURLOPT_URL, url.c_str()); //"http://www.iltalehti.fi/index.html");
    curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);

    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, curl_write);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, this);

    parser->begin();
    result = curl_easy_perform(handle);
    if (result != CURLE_OK) {
	Log::err("Web::read", curl_easy_strerror(result));
    }
    parser->end();
    Log::value("result", parser->result());
    delete parser;
    parser = NULL;
}

void Web::onData(const char* str)
{
    if (parser != NULL)
	parser->parse(str);

    if (verbose)
	Log::msg("Web::onData", str);
}

void Web::print()
{
}
