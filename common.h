#pragma once

#include <exception>
#include "infoitem.h"
#ifdef SCHEME
  #include "scmscript.h"
#else
  #include "luascript.h"
#endif

//------------------------------------------------------------------------------
class BaseRuntime
{
public:
    virtual void add(InfoReader* reader) = 0;

    virtual void addFunc(const char* name, foreign_func func) = 0;
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
#ifdef SCHEME
extern int arg_integer(scheme *sch, pointer arg);
#endif
