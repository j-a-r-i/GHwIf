/******************************************************************************
 * Copyright (C) 2018-9 Jari Ojanen
 ******************************************************************************/
#include "sun.h"
#include "common.h"
#include "logger.h"
#include <ctime>
#include <math.h>


Sun::Sun(IPluginScript& scm) :
    sunrise{scm, "sunrise"},
    sunset {scm, "sunset"}
{
}

/** based on http://edwilliams.org/sunrise_sunset_algorithm.htm
 */
void Sun::operator()()
{
#define LATITUDE  60.205490
#define LONGITUDE 24.655899
#define RISING 1
    double toDec = 180.0 / 3.1458;  // radians to degrees
    double latitude = atof(Cfg::get(CfgItem::LOCATION_LAT));
    double longitude= atof(Cfg::get(CfgItem::LOCATION_LON));
    
    time_t tim = time(NULL);
    tm* ts   = localtime(&tim);
    int month = ts->tm_mon + 1;
    int year  = ts->tm_year + 1900;
    int day   = ts->tm_mday;

    int N1 = static_cast<int>(floor(275.0 * month / 9.0));
    int	N2 = static_cast<int>(floor((month + 9) / 12.0));
    int	N3 = (1 + floor((year - 4 * floor(year / 4.0) + 2) / 3.0));
    int N = N1 - (N2 * N3) + day - 30;

    double lngHour = longitude / 15.0;
    double t;
    
    if (RISING)
	t = (6 - lngHour) / 24.0;	
    else
	t = (18 - lngHour) / 24.0;

    double M = (0.9856 * (t + N)) - 3.289;

    double L = M + (1.916 * toDec * sin(M/toDec)) + (0.020 * toDec * sin((2 * M)/toDec)) + 282.634;

    while (L > 360.0)
	L -= 360.0;

    double RA = toDec * atan((0.91764 * toDec * tan(L/toDec))/toDec);

    while (RA < 0.0)
	RA += 360.0;

    double Lquadrant  = (floor( L/90.0)) * 90;
    double RAquadrant = (floor(RA/90.0)) * 90;
    RA = RA + (Lquadrant - RAquadrant);

    
    
    Log::value("day",   day);
    Log::value("month", month);
    Log::value("year",  year);
    Log::value("lat",   latitude);
    Log::value("lon",   longitude);

    Log::value("n1", ts->tm_yday);
    Log::value("n2", N);
    Log::value("L",  L);
    Log::value("RA", RA);
}
