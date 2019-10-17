/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#pragma once

#include <config.h>
#include <exception>
#include <string>

#ifdef SCR_GUILE
  #include "libguile.h"
  typedef SCM (*foreign_func)(SCM a);
#endif
#ifdef SCR_SCHEME
  extern "C" {
        #include "tinyscheme/scheme-private.h"
        #include "tinyscheme/scheme.h"
  }
  typedef cell *(*foreign_func)(scheme *sc, cell *args);
#endif
#ifdef SCR_LUA
  extern "C" {
	#include "lua/lua.h"
	#include "lua/lauxlib.h"
	#include "lua/lualib.h"
  }
  typedef int(*foreign_func)(lua_State *l);
#endif
#ifdef SCR_PYTHON
  #include <Python.h>
  typedef PyObject *(*foreign_func)(PyObject *clss, PyObject *args);
  typedef PyObject* pointer;
#endif

#include "infoitem.h"

//------------------------------------------------------------------------------
class IPluginScript
{
public:
    virtual void exec(const char *func) = 0;
    virtual void eval(std::string& line) = 0;
    virtual void load(const char *filename) = 0;
    virtual void add(const char *name, foreign_func func) = 0;
    virtual pointer add(const char *name, double value) = 0;
};

//------------------------------------------------------------------------------
class IRuntime
{
public:
    virtual void command(const char *name) = 0;
    virtual void dbQuery(const char* sql) = 0;
};

//------------------------------------------------------------------------------
class ICommand
{
public:
    virtual const std::string& getName() = 0;
    virtual void execute() = 0;
};

//------------------------------------------------------------------------------
#ifdef SCR_SCHEME
  #include "scmscript.h"
#endif
/*#ifdef SCR_GUILE
  #include "guilescript.h"
#endif*/
#ifdef SCR_LUA
  #include "luascript.h"
#endif
#ifdef SCR_PYTHON
  #include "pyscript.h"
#endif

//------------------------------------------------------------------------------
struct Time {
	uint8_t weekday;
	uint8_t hour;
	uint8_t minute;
};

//------------------------------------------------------------------------------
class ISchedulerEvent
{
public:
	virtual void operator()() = 0;

	//virtual void calcNext(const Time& t) = 0;

protected:
	/** counter to next time calcNext is called.
	 */
	//uint16_t counter;
};


//------------------------------------------------------------------------------
class IScheduler
{
public:
	/** add new event to scheduler.
	 */
	virtual void add(ISchedulerEvent *event) = 0;

	/** every minute this method is called by timer to check all the schedule
	 *  handlers.
     */
	virtual void elapsed() = 0;
};

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
