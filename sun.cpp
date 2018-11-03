/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#include "sun.h"
#include "config.h"
#include "logger.h"


Sun::Sun() :
    InfoReader("sun"),
    sunrise("sunrise"),
    sunset("sunset")
{
    infos.push_back(&sunrise);
    infos.push_back(&sunset);
}

void Sun::read()
{
}

void Sun::print()
{
}
