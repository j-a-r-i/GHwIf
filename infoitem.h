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

    /** Get the name of the informational item
     */
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

    /** Set new current value for info item
     */
    void setValue(double v) {
	value = v;
    }

    /** Get current value
     */
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

    /** Prints information about info reader
     */
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
    /** The infomational elements that this readers is generating
     */
    std::list<InfoItem*> infos;
    
private:
    /** The name of info reader
     */
    std::string name;
};


