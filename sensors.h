/******************************************************************************
 * Copyright (C) 2016-9 Jari Ojanen
 ******************************************************************************/
#pragma once

#include "common.h"
#include <string>
#include <list>
#include <memory>

//------------------------------------------------------------------------------
class SensorItem : public InfoItemReal
{
public:
    SensorItem(IPluginScript& scm, const char *name, const char* chip, int feat);
    
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
class Sensors : public ISchedulerEvent
{
public:
    Sensors(IPluginScript& scm);

    virtual ~Sensors();

    void operator()();

private:
    std::list<std::unique_ptr<SensorItem>> sensors;
};
