/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#pragma once

#ifdef SCR_SCHEME

#include "tinyscheme/scheme-private.h"
#include "tinyscheme/scheme.h"

#define INIT_SCRIPT "test.scm"

/** interface to tinyscheme
 */
class ScmScript : public IPluginScript
{
public:
    
    ScmScript();
    virtual ~ScmScript();

    void addFn(const char* name, foreign_func func);

    void eval(const char* code);
    
    void exec(const char* func);
    
    void load(const char* filename);

    void mainLoop();

private:
    scheme scm;
};

extern int arg_integer(scheme *sch, cell *arg);
extern char *arg_string(scheme *sch, cell *arg);

class BaseRuntime;
void scm_func_init(BaseRuntime *rt);

typedef ScmScript Script;

#endif // SCR_SCHEME
