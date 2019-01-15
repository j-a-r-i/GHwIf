/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#pragma once

#include <config.h>
#include <exception>
#include "infoitem.h"

#ifdef SCR_GUILE
  #include "libguile.h"
  typedef SCM (*foreign_func)(SCM a);
#endif
#ifdef SCR_SCHEME
  #include "external/scheme-private.h"
  #include "external/scheme.h"
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

//------------------------------------------------------------------------------
class IPluginScript
{
public:
    virtual void exec(const char *func) = 0;
    virtual void eval(std::string& line) = 0;
    virtual void load(const char *filename) = 0;
    virtual void addFn(const char *name, foreign_func func) = 0;
};

//------------------------------------------------------------------------------
#ifdef SCR_SCHEME
  #include "scmscript.h"
#endif
#ifdef SCR_GUILE
  #include "guilescript.h"
#endif
#ifdef SCR_LUA
  #include "luascript.h"
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
class IRuntime
{
public:
    virtual void webLoad(int i, int arg) = 0;
    virtual void webGet(const char* url) = 0;
    virtual void webVerbose(bool value) = 0;
    virtual void dbQuery(const char* sql) = 0;
};

extern IRuntime *gRuntime;

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
