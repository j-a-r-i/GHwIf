/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#include "luascript.h"
#include "config.h"

#define MAX_STRING 80

char ConfigData[][MAX_STRING] = {
    [CFG_FMI_API]     = { FMI_API },
    [CFG_STRAVA_API]  = { STRAVA_API },
    [CFG_SCHEME_INIT] = { "init.scm" },
    [CFG_SERIAL_PORT] = { "/dev/ttyS0" },

    [CFG_MAX] = NULL
};


void Cfg::init()
{
}

const char* Cfg::get(CfgItem item)
{
}

void Cfg::set(CfgItem item, const char* value)
{
}
