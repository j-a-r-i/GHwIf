/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#include "config.h"
#include <string.h>

#define MAX_STRING 80

typedef struct {
    char value[MAX_STRING];
    bool priv;
} configitem_t;


configitem_t CfgData[] = {
    [CFG_FMI_API]     = { FMI_API,      true },
    [CFG_STRAVA_API]  = { STRAVA_API,   true },
    [CFG_SCHEME_INIT] = { "tinyscheme/init.scm", false },
    [CFG_SERIAL_PORT] = { "/dev/ttyS0", false },

    [CFG_MAX]         = { "",         true }
};


void Cfg::init()
{
}

const char* Cfg::get(CfgItem item)
{
    return CfgData[item].value;
}

void Cfg::set(CfgItem item, const char* value)
{
    strcpy(CfgData[item].value, value);
}
