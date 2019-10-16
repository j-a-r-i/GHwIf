/******************************************************************************
 * Copyright (C) 2016-8 Jari Ojanen
 ******************************************************************************/
#pragma once

#include <string>
#include <map>

#define VERSION_MAJOR @VERSION_MAJOR@
#define VERSINO_MINOR @VERSION_MINOR@

#define STR_FMI_API       "@FMI_API@"
#define STR_STRAVA_API    "@STRAVA_API@"
#define STR_SERIAL_PORT   "@SERIAL_PORT@" 
#define STR_SQLITE_DB     "@SQLITE_DB@"

//#define HW_RPI
//#define HW_LINUX
//#define SCR_GUILE
//#define SCR_PYTHON
#define SCR_SCHEME
//#define SCR_LUA
#define USE_SENSORS
//#define USE_BLUETOOTH
//#define USE_WS
//#define USE_JSON 
//#define USE_EXPAT


//#define SERIAL_PORT "/dev/ttyACM0"
//#define SERIAL_PORT "/dev/ttyS0"

#define PORT 8080
#define NUM_CONNECTIONS 10


enum class CfgItem {
    FMI_API = 0,
    STRAVA_API,
    SCRIPT_FILE,
    SERIAL_PORT,
    LOCATION_LAT,
    LOCATION_LON,
    SQLITE_DB,
	SCHEME_INIT,
	SCHEME_SYSTEM,
    MAX
};

class Cfg
{
public:
    static void init();
    static const char* get(CfgItem item);
    static void set(CfgItem item, const char* value);

    static void load();
    static void save();

private:
    static std::map<CfgItem, std::string> items;
};
