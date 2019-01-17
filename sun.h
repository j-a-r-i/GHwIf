/******************************************************************************
 * Copyright (C) 2018-9 Jari Ojanen
 ******************************************************************************/
#pragma once

#include "common.h"

/** Calculate sun rise and set. Based on 
 */
class Sun : public ISchedulerEvent
{
public:
    Sun(IPluginScript& scm);
    
    void operator()() override;

private:
    InfoItemReal sunrise;
    InfoItemReal sunset;
};
