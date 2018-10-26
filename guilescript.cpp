/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#include "guilescript.h"
#include "logger.h"

GuileScript::GuileScript()
{
    scm_init_guile();
}

void GuileScript::addFn(const char* name, guilefn func)
{
    scm_c_define_gsubr(name, 1, 0, 0, func);
}

double GuileScript::load(const char* fname)
{
    SCM func;
    
    scm_c_primitive_load(fname);

    func = scm_variable_ref(scm_c_lookup("main"));

    scm_call_p(func);
}

GuileScript::~GuileScript()
{
}
