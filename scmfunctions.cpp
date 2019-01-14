#include "common.h"

#define CHECK_RUNTIME() if (gRuntime == NULL) throw TheException(TheException::EMissingRuntime)

cell *scm_web_load(scheme *sch, cell *args)
{
    try {
	CHECK_RUNTIME();
	
	int i1 = arg_integer(sch, args);
	int i2 = arg_integer(sch, pair_cdr(args));

	gRuntime->webLoad(i1, i2);
    }
    catch (TheException& e) {
	Log::err(__FUNCTION__, e.what());
    }
    return sch->NIL;
}

cell *scm_web_get(scheme *sch, cell *args)
{
    try {
	char *s1 = arg_string(sch, args);

	gRuntime->webGet(s1);
    }
    catch (TheException& e) {
	Log::err(__FUNCTION__, e.what());
    }
    return sch->NIL;
}

cell *scm_web_verbose(scheme *sch, cell *args)
{
    try {
	int i = arg_integer(sch, args);

	gRuntime->webVerbose(i);
    }
    catch (TheException& e) {
	Log::err(__FUNCTION__, e.what());
    }
    return sch->NIL;
}

cell *scm_read_all(scheme *sch, cell *args)
{
    if (args == sch->NIL) {
	gRuntime->readAll();
    }
    else {
	Log::err(__FUNCTION__, "extra argument");
    }
    return sch->NIL;
}

cell *scm_dump(scheme *sch, cell *args)
{
    if (args == sch->NIL) {
	gRuntime->dump();
    }
    else {
	Log::err(__FUNCTION__, "extra argument");
    }
    return sch->NIL;
}

cell *scm_db_query(scheme *scm, cell *args)
{
    try {
	char *s1 = arg_string(scm, args);

	gRuntime->dbQuery(s1);
    }
    catch (TheException& e) {
	Log::err(__FUNCTION__, e.what());
    }
    return scm->NIL;
	
}

cell *scm_tst(scheme *scm, cell *args)
{
    try {
	printf("%d\n", ((pair_car(args)->flag) & 0x1F));
    }
    catch (TheException& e) {
	Log::err(__FUNCTION__, e.what());
    }
    return scm->NIL;
	
}

//------------------------------------------------------------------------------
void scm_func_init(IPluginScript *script)
{
    script->addFn("web-load",    scm_web_load);
    script->addFn("web-get",     scm_web_get);
    script->addFn("web-verbose", scm_web_verbose);
    script->addFn("read-all",    scm_read_all);
    script->addFn("dump",        scm_dump);
    script->addFn("db-query",    scm_db_query);
    script->addFn("tst",         scm_tst);
}
