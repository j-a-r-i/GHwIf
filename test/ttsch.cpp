#include <gtest/gtest.h>
#include <stdio.h>
#include "../logger.h"
#include "../tinyscheme/scheme-private.h"
#include "../tinyscheme/scheme.h"

const char *SCM1 = R"(
(define items '("one" "two" "three"))

(display2 "foo")
(display2 123)
(for-each
  (lambda (x)
     (display " - ")
     (display x)
     (display "\n"))
  items)
)";
    
pointer display2(scheme *sch, pointer args)
{
    if (args != sch->NIL) {
	pointer car = pair_car(args);
	if (is_string(car)) {
	    char *str = string_value(car);
	    printf("<%s>\n", str);
	}
	else if (is_integer(car)) {
	    printf("<%d>\n", ivalue(car));
	}
    }
}

/*
SCM lget(SCM *a)
{
    lua_pushnumber(l, 12.0);
    return 1;
}
*/

/** test return value from lua to c
 */
TEST(tinysch, ret_val)
{
    scheme sch;

    if (!scheme_init(&sch)) {
	Log::err("scheme", "init");
	return;
    }

    FILE *finit = fopen("../tinyscheme/init.scm", "r");
    scheme_load_file(&sch, finit);
    fclose(finit);

    scheme_set_input_port_file(&sch, stdin);
    scheme_set_output_port_file(&sch, stdout);

    scheme_define(&sch, sch.global_env,
		  mk_symbol(&sch,      "display2"),
		  mk_foreign_func(&sch,display2)); 
    
    scheme_load_string(&sch, SCM1);

    scheme_deinit(&sch);
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
