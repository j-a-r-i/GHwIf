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

protected:
	/* container for all the info items (if needed) */
	//static std::list<InfoItem*> infos;

private:
    std::string name;
};

class IPluginScript;
struct cell;

/** Store double values
 */
class InfoItemReal : public InfoItem
{
public:
	InfoItemReal(IPluginScript& scm, const char* n);

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
	cell *scmValue;
};
