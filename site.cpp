#include "site.h"


SiteBase::SiteBase(const std::string u, const std::string p) :
    url{u},
    path{p}
{
}

SiteBase::~SiteBase
{
}

const std::string SiteBase::getUrl()
{
}

//------------------------------------------------------------------------------
NasdaqSite::NasdaqSite() :
    SiteBase{"www.nasdaqomxnordic.com", "/webproxy/DataFeedProxy.aspx"}
{
    query = {{"Subsystem", "History"},
	     {"Action",    "GetDataSeries"},
	     {"Instrument","HEX1111"},
	     {"FromDate",  "2018-12-01"}
    };
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
    query = {{"lat", Cfg::get(CfgItem::LOCATION_LAT)},
	     {"lng", Cfg::get(CfgItem::LOCATION_LON)}
    };
}

