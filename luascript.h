/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#pragma once

extern "C" {
#include "lua/src/lua.h"
#include "lua/src/lauxlib.h"
#include "lua/src/lualib.h"
}
    
typedef int (*foreign_func)(lua_State *l);

class LuaScript
{
public:
    LuaScript();
    virtual ~LuaScript();

    void addFn(const char* name, foreign_func func);

    void exec(const char *func);
    
    double load(const char* filename);

    void mainLoop();

private:
    lua_State *lua;
};

typedef LuaScript Script;
