/******************************************************************************
 * Copyright (C) 2016-8 Jari Ojanen
 ******************************************************************************/
#pragma once

#include "infoitem.h"

class Sensors : public InfoItem
{
public:
    Sensors();

    virtual ~Sensors();

    void read();
private:
};
