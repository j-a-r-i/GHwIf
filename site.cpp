#include "config.h"
#include "site.h"


SiteBase::SiteBase(const std::string u, const std::string p) :
    url{u},
    path{p}
{
}

SiteBase::~SiteBase()
{
}

const std::string SiteBase::getUrl() const
{
    return url;
}

//------------------------------------------------------------------------------
NasdaqSite::NasdaqSite() :
    SiteBase{"www.nasdaqomxnordic.com", "/webproxy/DataFeedProxy.aspx"}
{
    query.insert(std::make_pair("Subsystem", "History"));
    query.insert(std::make_pair("Action",    "GetDataSeries"));
    query.insert(std::make_pair("Instrument","HEX1111"));
    query.insert(std::make_pair("FromDate",  "2018-12-01"));
}

//------------------------------------------------------------------------------
FmiSite::FmiSite() :
    SiteBase{"data.fmi.fi", "/fmi-apikey/"}  // note need to append api key
{
    query = {{"request",        "getFeature"},
	     {"storedquery_id", "fmi::forecast::hirlam::surface::point::multipointcoverage"},
	     {"place",          "oittaa"},
	     {"parameters",     "temperature,dewpoint,windspeedms,precipitation1h"}
    };
}

//------------------------------------------------------------------------------
StravaSite::StravaSite() :
    SiteBase{"www.strava.com", "/api/v3/athlete/activities"}
{
    query = {{"page", "1"},
	     {"access_token", Cfg::get(CfgItem::STRAVA_API)}
    };
}

//------------------------------------------------------------------------------
SunriseSite::SunriseSite() :
    SiteBase{"api.sunrise-sunset.org", "/json"}
{
    query.insert(std::make_pair("lat", Cfg::get(CfgItem::LOCATION_LAT)));
    query.insert(std::make_pair("lng", Cfg::get(CfgItem::LOCATION_LON)));
}

