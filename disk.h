/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#pragma once

#include "infoitem.h"

class Disk : public InfoItem
{
public:
    Disk();

    void read();

private:
    void readDisk(const char* mountpoint);
};
