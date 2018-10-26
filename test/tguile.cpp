#include <gtest/gtest.h>
#include "../guilescript.h"

/*
SCM lget(SCM *a)
{
    lua_pushnumber(l, 12.0);
    return 1;
}
*/

/** test return value from lua to c
 */
TEST(guile, ret_val)
{
    GuileScript guile;
    double value = guile.load("test1.scm");

    EXPECT_EQ(value, 123.0);
}

/** test calling C function from lua.
 *
TEST(guile, call_c)
{
    LuaScript lua;
    double value;

    lua.addFn("get", lget);
    
    value = lua.load("test2.lua");
    
    EXPECT_EQ(value, 12.0);
    }*/
