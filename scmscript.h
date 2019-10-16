/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#pragma once

#ifdef SCR_SCHEME

#include "scheme-private.h"
#include "scheme.h"

#define INIT_SCRIPT "test.scm"

/** interface to tinyscheme
 */
class ScmScript : public IPluginScript
{
public:
    
    ScmScript(IRuntime *rt);
    virtual ~ScmScript();

    void add(const char* name, foreign_func func) override;
	pointer add(const char* name, double value) override;

    void eval(std::string& line);
    
    void exec(const char* func);
    
    void load(const char* filename);

    void mainLoop();

	void version();

private:
    scheme scm;
};

extern int arg_integer(scheme *sch, cell *arg);
extern char *arg_string(scheme *sch, cell *arg);

void scm_func_init(IPluginScript *script);

typedef ScmScript Script;

#endif // SCR_SCHEME
