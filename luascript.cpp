/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#include "luascript.h"
#include "logger.h"
#include <string.h>

/*
** these libs are loaded by lua.c and are readily available to any Lua
** program
*/
static const luaL_Reg loadedlibs[] = {
  {"_G", luaopen_base},
  {LUA_LOADLIBNAME, luaopen_package},
  {LUA_COLIBNAME, luaopen_coroutine},
  {LUA_TABLIBNAME, luaopen_table},
  {LUA_STRLIBNAME, luaopen_string},
  {LUA_DBLIBNAME, luaopen_debug},
  {NULL, NULL}
};


LuaScript::LuaScript()
{
    lua = luaL_newstate();

    const luaL_Reg *lib;
    /* "require" functions from 'loadedlibs' and set results to global table */
    for (lib = loadedlibs; lib->func; lib++) {
	luaL_requiref(lua, lib->name, lib->func, 1);
	lua_pop(lua, 1);  /* remove lib */
    }
}

void LuaScript::addFn(const char* name, foreign_func func)
{
    lua_pushcfunction(lua, func);
    lua_setglobal(lua, name);
}

void LuaScript::eval(const char *line)
{
    int error = luaL_loadbuffer(lua,
				line,
				strlen(line),
				"line");

    if (!error)
	error = lua_pcall(lua, 0, 0, 0);

    if (error) {
	Log::err("lua::eval", lua_tostring(lua, -1));
	lua_pop(lua, 1);                // pop error message from the stack
    }
}

void LuaScript::exec(const char *fname)
{
    lua_getglobal(lua, fname);
    lua_pushnumber(lua, 12);
    lua_pushnumber(lua, 32);

    if (lua_pcall(lua, 2, 0, 0) != 0) {
	Log::err("lua::pcall", "fail");
    }	
}

double LuaScript::load(const char* fname)
{
    int error = luaL_dofile(lua, fname);
    double ret;
    
    if (error) {
	Log::err("lua::load", luaL_checkstring(lua, -1));
    }

    ret = lua_tonumber(lua, -1);
    lua_pop(lua, 1);

    //Log::value("return", ret);

    return ret;
}

void LuaScript::mainLoop()
{
    load("init.lua");
}

LuaScript::~LuaScript()
{
    lua_close(lua);
}
