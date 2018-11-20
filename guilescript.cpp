/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#include "common.h"
#include "guilescript.h"
#include "logger.h"

GuileScript::GuileScript()
{
    scm_init_guile();
}

void GuileScript::addFn(const char* name, foreign_func func)
{
    scm_c_define_gsubr(name, 1, 0, 0, (void*)func);
}

void GuileScript::eval(const char* code)
{
    scm_c_eval_string(code);
}

void GuileScript::exec(const char* funcname)
{
    SCM func;

    func = scm_variable_ref(scm_c_lookup(funcname));

    scm_call_0(func );
}


void GuileScript::load(const char* fname)
{
    scm_c_primitive_load(fname);
}

GuileScript::~GuileScript()
{
}


void scm_func_init(BaseRuntime *rt)
{
}
