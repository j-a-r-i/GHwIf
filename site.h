#pragma once

#include <string>
#include <map>
#include "xmlparsesimple.h"

class SiteBase
{
public:
    SiteBase(const std::string u, const std::string p);
    ~SiteBase();

    const std::string getUrl() const;

    BaseParse* getParser() const {
	return parser;
    }
    
protected:
    const std::string url;
    const std::string path;
    std::map<std::string, std::string> query;

    BaseParse *parser;
};


class NasdaqSite : public SiteBase
{
public:
    NasdaqSite();
    
private:
};


class FmiSite : public SiteBase
{
public:
    FmiSite();
    
private:
};


class StravaSite : public SiteBase
{
public:
    StravaSite();
    
private:
};


class SunriseSite : public SiteBase
{
public:
    SunriseSite();
    
private:
};
