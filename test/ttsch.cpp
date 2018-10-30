#include <gtest/gtest.h>
#include <stdio.h>
#include "../logger.h"
#include "../config.h"
#include "../scmscript.h"

const char *SCM1 = R"(
(define items '("one" "two" "three"))

(display2 "foo")
(display2 (cfg-get 2))
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

pointer cfg_get(scheme *sch, pointer args)
{
    if (args != sch->NIL) {
	pointer car = pair_car(args);
	if (is_integer(car)) {
	    int val = ivalue(car);
	    //printf("<%d>\n", val);
	    return mk_string(sch, Cfg::get((CfgItem)val));
	}
    }
    return sch->NIL;
}

/** test return value from lua to c
 */
TEST(tinysch, ret_val)
{
    Cfg::set(CFG_SCHEME_INIT, "../tinyscheme/init.scm");
    ScmScript scm;

    scm.addFn("display2", display2);
    scm.addFn("cfg-get",  cfg_get);

    scm.exec(SCM1);
}
