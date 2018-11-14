/******************************************************************************
 * Copyright (C) 2016-8 Jari Ojanen
 ******************************************************************************/
#pragma once

#include "infoitem.h"
#include <string>
#include <list>

//------------------------------------------------------------------------------
class SensorItem : public InfoItemReal
{
public:
    SensorItem(const char *name, const char* chip, int feat);
    
    const std::string& getChipName() const {
	return chipName;
    }

    int getFeature() const {
	return subFeature;
    }
    
private:
    std::string chipName;
    int subFeature;
};

//------------------------------------------------------------------------------
class Sensors : public InfoReader
{
public:
    Sensors();

    virtual ~Sensors();

    void read();
    void print();
};
