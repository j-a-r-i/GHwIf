/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#include "common.h"
#include "web.h"
#include "xmlparsesimple.h"
#ifdef HW_LINUX
  #include "htmlparse.h"
#endif
#include "logger.h"
#include <sstream>

//#define USE_CURL

#define SITE_NASDAQ  "www.nasdaqomxnordic.com"
#define SITE_FMI     "data.fmi.fi"
#define SITE_STRAVA  "www.strava.com"
#define SITE_SUNRISE "api.sunrise-sunset.org"
#define SITE_DWEET   "dweet.io"
#define SEP "&"

size_t curl_write(void *buffer, size_t size, size_t nmemb, void *user_data)
{
    Web *self = (Web*)user_data;

    if (self != NULL)
	self->onData((const char*)buffer);

    return size * nmemb;
}

Web::Web(bool v) 
{
#ifdef USE_CURL
    curl_global_init(CURL_GLOBAL_ALL);

    handle = curl_easy_init();
#else
	handle = NULL;
#endif
    parser = NULL;

    verbose = v;
}

Web::~Web()
{
    if (parser != NULL)
		delete parser;

#ifdef USE_CURL    
    if (handle != NULL)
		curl_easy_cleanup(handle);
    
    curl_global_cleanup();
#endif
}

void Web::operator()()
{
    if (handle == NULL) {
		Log::err("Web::read", "CURL not initialized");
		return;
    }
    if (parser == NULL) {
		Log::err("Web::read", "parser is not initialized");
		return;
    }

#ifdef USE_CURL
    CURLcode result;

    Log::msg("web", url.c_str());
    
    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
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
#endif
}

void Web::onData(const char* str)
{
    if (parser != NULL)
	parser->parse(str);

    if (verbose)
	Log::msg("Web::onData", str);
}

void Web::version()
{
    Log::msg("version curl  ", LIBCURL_VERSION);
}

//------------------------------------------------------------------------------
WebNasdaqHist::WebNasdaqHist(int instrument) :
    Web(false)
{
//SiteArg 24311 = NOKIA
    std::ostringstream os;

    os << "https://" << SITE_NASDAQ
       << "/webproxy/DataFeedProxy.aspx?"
       << "Subsystem="     << "History" << SEP
       << "Action=Get"     << "DataSeries" << SEP
       << "Instrument=HEX" << instrument << SEP
       << "FromDate="      << "2019-09-24";

    parser = new XmlParseTag("hi", "cp");
    url = os.str();
}

//------------------------------------------------------------------------------
WebNasdaqLast::WebNasdaqLast(int instrument) :
    Web(false)
{
    std::ostringstream os;
    os << "https://" << SITE_NASDAQ
       << "/webproxy/DataFeedProxy.aspx?"
       << "Subsystem="     << "Prices" << SEP
       << "Action=Get"     << "Instrument" << SEP
       << "Instrument=HEX" << instrument;

    parser = new XmlParseTag("inst", "lsp");
    url = os.str();
}

//------------------------------------------------------------------------------
WebFmi::WebFmi() :
    Web(false)
{
    std::ostringstream os;
    os << "http://" << SITE_FMI
       << "/fmi-apikey/"   << Cfg::get(CfgItem::FMI_API) << "/wfs?"
       << "request="        << "getFeature" << SEP
       << "storedquery_id=" << "fmi::forecast::hirlam::surface::point::multipointcoverage" << SEP
       << "place="          << "oittaa" << SEP
       << "parameters="     << "temperature,dewpoint,windspeedms,precipitation1h";

    parser = new XmlParseTag("hi", "cp");
    url = os.str();
}

//------------------------------------------------------------------------------
WebStrava::WebStrava() :
    Web(false)
{
    std::ostringstream os;
    os << "https://" << SITE_STRAVA 
       << "/api/v3/athlete/activities?"
       << "page="   << 1 << SEP
       << "access_token=" << Cfg::get(CfgItem::STRAVA_API) << SEP;
    
    parser = new DummyParse();
    url = os.str();
}

//------------------------------------------------------------------------------
WebSunrise::WebSunrise() :
    Web(false)
{
    std::ostringstream os;
    os << "https://" << SITE_SUNRISE
       << "/json?"
       << "lat=" << Cfg::get(CfgItem::LOCATION_LAT) << SEP
       << "lng=" << Cfg::get(CfgItem::LOCATION_LON);
      //<< "date=" << "today";

    parser = new DummyParse();
    url = os.str();
}

//------------------------------------------------------------------------------
WebCustom::WebCustom(const char* site, const char* tag) :
    Web(false)
{
#ifdef HW_LINUX
    parser = new HtmlParse();
#endif
    url = site;
}
