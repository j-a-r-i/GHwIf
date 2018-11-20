/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#pragma once

#include "libguile.h"

#define INIT_SCRIPT "guile.scm"

typedef SCM (*foreign_func)(SCM a);

/** interface to guile
 */
class GuileScript : public IPluginScript
{
public:
    GuileScript();
    virtual ~GuileScript();

    void addFn(const char* name, foreign_func func);

    void eval(const char* code);

    void exec(const char* func);
    
    void load(const char* filename);

private:
    SCM *guile;
};

class BaseRuntime;
void scm_func_init(BaseRuntime *rt);

typedef GuileScript Script;
