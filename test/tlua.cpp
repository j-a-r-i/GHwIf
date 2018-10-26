#include <gtest/gtest.h>
#include "../luascript.h"

int lget(lua_State *l)
{

    lua_pushnumber(l, 12.0);
    return 1;
}

/** test return value from lua to c
 */
TEST(lua, ret_val)
{
    LuaScript lua;
    double value = lua.load("test1.lua");

    EXPECT_EQ(value, 123.0);
}

/** test calling C function from lua.
 */
TEST(lua, call_c)
{
    LuaScript lua;
    double value;

    lua.addFn("get", lget);
    
    value = lua.load("test2.lua");
    
    EXPECT_EQ(value, 12.0);
}
