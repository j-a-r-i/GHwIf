/******************************************************************************
 * Copyright (C) 2016-8 Jari Ojanen
 ******************************************************************************/
#pragma once

#include <string>

class InfoItem
{
public:
    InfoItem(const char* n) : name(n) {
    }
    
    //virtual ~InfoItem() = 0;

    virtual void read() = 0;
    virtual void print() = 0;
private:
    std::string name;
};
