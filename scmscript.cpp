/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#include "logger.h"
#include "common.h"
#ifdef USE_READLINE
  #include <readline/readline.h>
#endif

//------------------------------------------------------------------------------
int arg_integer(scheme *sch, cell *arg)
{
    int retVal = 0;
    
    if (arg != sch->NIL) {
	cell *car = pair_car(arg);
	if (is_integer(car)) {
	    retVal = ivalue(car);
	}
	else {
	    throw TheException(TheException::EArgumentType);
	}
    }
    else {
	throw TheException(TheException::EMissingArgument);
    }
    return retVal;
}

char *arg_string(scheme *sch, cell *arg)
{
    char *retVal = NULL;
    
    if (arg != sch->NIL) {
	cell *car = pair_car(arg);
	if (is_string(car)) {
	    retVal = string_value(car);
	}
	else {
	    throw TheException(TheException::EArgumentType);
	}
    }
    else {
	throw TheException(TheException::EMissingArgument);
    }
    return retVal;
}

//------------------------------------------------------------------------------
ScmScript::ScmScript()
{
    Log::msg("scm", "init");
    if (!scheme_init(&scm)) {
		Log::err("scheme", "init");
		return;
    }

    load(Cfg::get(CfgItem::SCHEME_INIT));
    load(Cfg::get(CfgItem::SCHEME_SYSTEM));

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

void ScmScript::eval(const char* line)
{
    scheme_load_string(&scm, line);
}

void ScmScript::exec(const char* func)
{
    scheme_apply0(&scm, func);
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
    //scheme_load_named_file(&scm, stdin, 0);

#ifdef USE_READLINE
    while (1) {
	char output[256];
	char *line = readline("ha>");

	output[0] = 0;
	Log::msg("eval", line);
	scheme_set_output_port_string(&scm, output, output+256);
	scheme_load_string(&scm, line);
	//sc->output.
	Log::msg("ret", output);
	
	
	delete(line);
    }
#endif
}
