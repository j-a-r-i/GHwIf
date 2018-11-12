/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#pragma once

#include "tinyscheme/scheme-private.h"
#include "tinyscheme/scheme.h"

/** interface to tinyscheme
 */
class ScmScript
{
public:
    
    ScmScript();
    virtual ~ScmScript();

    void addFn(const char* name, foreign_func func);

    void exec(const char* code);
    
    void load(const char* filename);

    void mainLoop();

private:
    scheme scm;
};

typedef Script ScmScript;
