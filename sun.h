/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#pragma once

#include "infoitem.h"

/** Calculate sun rise and set. Based on 
 */
class Sun : public InfoReader
{
public:
    Sun();
    
    void read();
    void print();

private:
    InfoItem sunrise;
    InfoItem sunset;
};
