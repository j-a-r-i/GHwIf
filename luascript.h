/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#pragma once

#ifdef SCR_LUA

extern "C" {
#include "lua/src/lua.h"
#include "lua/src/lauxlib.h"
#include "lua/src/lualib.h"
}

#define INIT_SCRIPT "init.lua"

typedef int (*foreign_func)(lua_State *l);

class LuaScript : public IPluginScript
{
public:
    LuaScript();
    virtual ~LuaScript();

    void addFn(const char* name, foreign_func func);

    void eval(const char *line);
    
    void exec(const char *func);
    
    void load(const char* filename);

    void mainLoop();

private:
    lua_State *lua;
};

typedef LuaScript Script;

#endif // SCR_LUA
