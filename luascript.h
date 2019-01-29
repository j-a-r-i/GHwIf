/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#pragma once

#ifdef SCR_LUA

extern "C" {
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"
}

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
