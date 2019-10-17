#include "common.h"

IRuntime *get_runtime(scheme* scm)
{
	IRuntime *rt = static_cast<IRuntime*>(scm->ext_data);
	if (rt == NULL)
		throw TheException(TheException::EMissingRuntime);

	return rt;
}

/*cell *scm_web_load(scheme *scm, cell *args)
{
    try {
		IRuntime *rt = get_runtime(scm);
		int i1 = arg_integer(scm, args);
		int i2 = arg_integer(scm, pair_cdr(args));

		rt->webLoad(i1, i2);
    }
    catch (TheException& e) {
		Log::err(__FUNCTION__, e.what());
    }
    return scm->NIL;
}

cell *scm_web_get(scheme *scm, cell *args)
{
    try {
		IRuntime *rt = get_runtime(scm);
		char *s1 = arg_string(scm, args);

		rt->webGet(s1);
    }
    catch (TheException& e) {
		Log::err(__FUNCTION__, e.what());
    }
    return scm->NIL;
}

cell *scm_web_verbose(scheme *scm, cell *args)
{
    try {
		IRuntime *rt = get_runtime(scm);
		int i = arg_integer(scm, args);

		rt->webVerbose(i);
    }
    catch (TheException& e) {
		Log::err(__FUNCTION__, e.what());
    }
    return scm->NIL;
}*/

cell *scm_command(scheme *scm, cell *args)
{
    try {
		IRuntime *rt = get_runtime(scm);
		char *s1 = arg_string(scm, args);

		rt->command(s1);
    }
    catch (TheException& e) {
		Log::err(__FUNCTION__, e.what());
    }
    return scm->NIL;
}


cell *scm_read_all(scheme *scm, cell *args)
{
    if (args == scm->NIL) {
		IRuntime *rt = get_runtime(scm);
		//rt->readAll();
    }
    else {
		Log::err(__FUNCTION__, "extra argument");
    }
    return scm->NIL;
}

cell *scm_db_query(scheme *scm, cell *args)
{
    try {
		IRuntime *rt = static_cast<IRuntime*>(scm->ext_data);
		if (rt == NULL)
			throw TheException(TheException::EMissingRuntime);

		char *s1 = arg_string(scm, args);

		rt->dbQuery(s1);
    }
    catch (TheException& e) {
		Log::err(__FUNCTION__, e.what());
    }
    return scm->NIL;
	
}

cell *scm_tst(scheme *scm, cell *args)
{
    try {
		printf("%d\n", ((pair_car(args)->_flag) & 0x1F));
    }
    catch (TheException& e) {
		Log::err(__FUNCTION__, e.what());
    }
    return scm->NIL;
	
}

//------------------------------------------------------------------------------
void scm_func_init(IPluginScript *script)
{
//    script->add("web-load",    scm_web_load);
//    script->add("web-get",     scm_web_get);
//    script->add("web-verbose", scm_web_verbose);
    script->add("cmd",         scm_command);
    script->add("read-all",    scm_read_all);
    script->add("db-query",    scm_db_query);
    script->add("tst",         scm_tst);
}
