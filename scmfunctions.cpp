#include "common.h"

IRuntime *get_runtime(scheme* scm)
{
	IRuntime *rt = static_cast<IRuntime*>(scm->ext_data);
	if (rt == NULL)
		throw TheException(TheException::EMissingRuntime);

	return rt;
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
    script->add("read-all",    scm_read_all);
    script->add("db-query",    scm_db_query);
    script->add("tst",         scm_tst);
}
