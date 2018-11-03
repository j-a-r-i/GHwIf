#pragma once

#include <exception>
#include "infoitem.h"
#include "scmscript.h"

//------------------------------------------------------------------------------
class BaseRuntime
{
public:
    virtual void add(InfoReader* reader) = 0;

    virtual void addFunc(const char* name, scmfn func) = 0;
};

//------------------------------------------------------------------------------
class TheException : public std::exception
{
public:
    enum Error {
	EMissingArgument,
	EArgumentType,
    };
    
    TheException(Error e) :
	error(e)
    {
    }
    
    const char* what() const throw ();

private:
    Error error;
};


//------------------------------------------------------------------------------
extern int arg_integer(scheme *sch, pointer arg);
