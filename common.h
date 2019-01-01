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
  #include "tinyscheme/scheme-private.h"
  #include "tinyscheme/scheme.h"
  typedef cell *(*foreign_func)(scheme *sc, cell *args);
#endif
#ifdef SCR_LUA
  extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
  }
  typedef int(*foreign_func)(lua_State *l);
#endif

//------------------------------------------------------------------------------
class IPluginScript
{
public:
    virtual void exec(const char *func) = 0;
    virtual void eval(const char *line) = 0;
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
class BaseRuntime
{
public:
    BaseRuntime(IPluginScript &scr) :
        script(scr)
    {
    }

    IPluginScript &script;
    
    virtual void add(InfoReader* reader) = 0;
    virtual void addFunc(const char* name, foreign_func func) = 0;

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
