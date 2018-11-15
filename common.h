#pragma once

#include "config.h"
#include <exception>
#include "scmscript.h"
#include "infoitem.h"

//------------------------------------------------------------------------------
class BaseRuntime
{
public:
    virtual void add(InfoReader* reader) = 0;
    virtual void addFunc(const char* name, foreign_func func) = 0;

    virtual void scr_run(const char* func) = 0;
    virtual void scr_eval(const char* line) = 0;
    
    virtual void readAll() = 0;
    virtual void dump() = 0;
    virtual void webLoad(int i, int arg) = 0;
    virtual void webGet(const char* url) = 0;
    virtual void webVerbose(bool value) = 0;
    virtual void dbQuery(const char* sql) = 0;
};

extern BaseRuntime *gRuntime;

//------------------------------------------------------------------------------
class TheException : public std::exception
{
public:
    enum Error {
	EMissingArgument,
	EArgumentType,
	ETidyParseBuffer,
	ETidyCleanNRepair,
	ETidyRunDiagnostics,
	EMissingRuntime
    };
    
    TheException(Error e) :
	error(e)
    {
    }
    
    const char* what() const throw ();

private:
    Error error;
};
