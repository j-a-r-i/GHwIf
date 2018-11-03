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
    void setValue(double v) {
	value = v;
    }
    double getValue() const {
	return value;
    }
private:
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
private:
    std::list<SensorItem*> items;
};
