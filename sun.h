/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#pragma once

#include "common.h"

/** Calculate sun rise and set. Based on 
 */
class Sun : public ISchedulerEvent
{
public:
    Sun();
    
    void operator()() override;

private:
    InfoItem sunrise;
    InfoItem sunset;
};
