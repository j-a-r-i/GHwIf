/******************************************************************************
 * Copyright (C) 2016-8 Jari Ojanen
 ******************************************************************************/
#pragma once

#include "infoitem.h"
#include <string>
#include <list>

//------------------------------------------------------------------------------
class SensorItem : public InfoItem
{
public:
    SensorItem(const char *name, const char* chip, int feat);
    
    void setValue(double v) {
	value = v;
    }
    double getValue() const {
	return value;
    }

    const std::string& getChipName() const {
	return chipName;
    }

    int getFeature() const {
	return subFeature;
    }
    
private:
    std::string chipName;
    int subFeature;
    
    double value;
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
