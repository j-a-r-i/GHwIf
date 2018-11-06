/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#include "sun.h"
#include "config.h"
#include "logger.h"
#include <ctime>
#include <math.h>


Sun::Sun() :
    InfoReader("sun"),
    sunrise("sunrise"),
    sunset("sunset")
{
    infos.push_back(&sunrise);
    infos.push_back(&sunset);
}

/** based on http://edwilliams.org/sunrise_sunset_algorithm.htm
 */
void Sun::read()
{
#define LATITUDE  60.205490
#define LONGITUDE 24.655899
    
    time_t t = time(NULL);
    tm* ts   = localtime(&t);
    int month = ts->tm_mon + 1;
    int year  = ts->tm_year + 1900;
    int day   = ts->tm_mday;

    int N1 = floor(275 * month / 9);
    int	N2 = floor((month + 9) / 12);
    int	N3 = (1 + floor((year - 4 * floor(year / 4) + 2) / 3));
    int N = N1 - (N2 * N3) + day - 30;
    

    Log::value("day",   day);
    Log::value("month", month);
    Log::value("year",  year);

    Log::value("n1", ts->tm_yday);
    Log::value("n2", N);
}

void Sun::print()
{
}
