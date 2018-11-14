/******************************************************************************
 * Copyright (C) 2016-8 Jari Ojanen
 ******************************************************************************/
#pragma once

#include <string>
#include <list>
#include "logger.h"

/** Store for informational element, the base class.
 */
class InfoItem
{
public:
    InfoItem(const char* n) : name(n) {
    }

    const std::string& getName() const {
	return name;
    }

    void dump() {
	Log::msg(": - ", name.c_str());
    }
 
private:
    std::string name;
};

/** Store double values
 */
class InfoItemReal : public InfoItem
{
public:
    InfoItemReal(const char* n) : InfoItem(n) {
    }
    
    void setValue(double v) {
	value = v;
    }
    double getValue() const {
	return value;
    }

private:
    double value;
};

/** Reader for informational element. Can read one or several data items.
 */
class InfoReader
{
public:
    InfoReader(const char* n) : name(n) {
    }

    void dump() {
	Log::msg(":", name.c_str());
	for (auto info : infos) {
	    info->dump();
	}
    }
    
    //virtual ~InfoReader() = 0;

    virtual void read() = 0;
    virtual void print() = 0;

protected:
    // does not own the infoitems
    std::list<InfoItem*> infos;
    
private:
    std::string name;
};


