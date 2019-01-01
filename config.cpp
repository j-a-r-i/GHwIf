/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#include "config.h"
#include <string.h>
#include <fstream>

/*#define MAX_STRING 80

typedef struct {
    char value[MAX_STRING];
    bool priv;
} configitem_t;


configitem_t CfgData[] = {
    [CFG_FMI_API]     = { FMI_API,           true },
    [CFG_STRAVA_API]  = { STRAVA_API,        true },
    [CFG_SCHEME_INIT] = { "lisp/init.scm",   false },
    [CFG_SCHEME_SYSTEM]={ "lisp/system.scm", false },
    [CFG_SERIAL_PORT] = { "/dev/ttyS0",      false },
    [CFG_LOCATION_LAT]= { "60.205490",       false },
    [CFG_LOCATION_LON]= { "24.655899",       false },
    [CFG_SQLITE_DB]   = { "data.db",         false },
    [CFG_MAX]         = { "",                true }
};*/

std::map<CfgItem, std::string> Cfg::items = {
	{ CfgItem::FMI_API,       STR_FMI_API },
	{ CfgItem::STRAVA_API,    STR_STRAVA_API },
	{ CfgItem::SCHEME_INIT,   "lisp/init.scm" },
	{ CfgItem::SCHEME_SYSTEM, "lisp/system.scm" },
	{ CfgItem::SERIAL_PORT,   "/dev/ttyS0" },
	{ CfgItem::LOCATION_LAT,  "60.205490" },
	{ CfgItem::LOCATION_LON,  "24.655899" },
	{ CfgItem::SQLITE_DB,     STR_SQLITE_DB }
};


void Cfg::init()
{
}

const char* Cfg::get(CfgItem item)
{
	return items[item].c_str();
}

void Cfg::set(CfgItem item, const char* value)
{
	items[item] = value;
}

void Cfg::load()
{

}

void Cfg::save()
{
    std::ofstream file{ "config.txt" };

    for (auto& [key,value] : items) {
	 file << static_cast<int>(key) << " " << value.c_str() << std::endl;
    }
    file.close();
}
