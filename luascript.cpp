/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#include "luascript.h"
#include "logger.h"


LuaScript::LuaScript()
{
    lua = luaL_newstate();

    luaL_openlibs(lua);
}

void LuaScript::addFn(const char* name, foreign_func func)
{
    lua_pushcfunction(lua, func);
    lua_setglobal(lua, name);
}

void LuaScript::exec(const char *fname)
{
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
}

LuaScript::~LuaScript()
{
    lua_close(lua);
}
