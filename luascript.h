/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#pragma once

extern "C" {
#include "lua/src/lua.h"
#include "lua/src/lauxlib.h"
#include "lua/src/lualib.h"
}
    
typedef int (*luafn)(lua_State *l);

class LuaScript
{
public:
    LuaScript();
    virtual ~LuaScript();

    void addFn(const char* name, luafn func);
    
    double load(const char* filename);

private:
    lua_State *lua;
};
