/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#include "scmscript.h"
#include "logger.h"
#include "config.h"

ScmScript::ScmScript()
{
    Log::msg("scm", "init");
    if (!scheme_init(&scm)) {
	Log::err("scheme", "init");
	return;
    }

    load(Cfg::get(CFG_SCHEME_INIT));
    load(Cfg::get(CFG_SCHEME_SYSTEM));

    scheme_set_input_port_file(&scm, stdin);
    scheme_set_output_port_file(&scm, stdout);
}

ScmScript::~ScmScript()
{
    Log::msg("scm", "deinit");
    scheme_deinit(&scm);
}

void ScmScript::addFn(const char* name, foreign_func func)
{
    scheme_define(&scm, scm.global_env,
		  mk_symbol(&scm,      name),
		  mk_foreign_func(&scm,func)); 
}

void ScmScript::exec(const char* code)
{
    scheme_load_string(&scm, code);
}

void ScmScript::load(const char* filename)
{
    Log::msg("scm load", filename);
    
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
	Log::err("scheme open", filename);
	return;
    }
    scheme_load_named_file(&scm, f, filename); 
    fclose(f);
}

void ScmScript::mainLoop()
{
    scheme_load_named_file(&scm, stdin, 0);
}
