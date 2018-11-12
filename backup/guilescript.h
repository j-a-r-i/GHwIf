/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#pragma once

#include <libguile.h>

typedef SCM (*guilefn)(SCM a);

class GuileScript
{
public:
    GuileScript();
    virtual ~GuileScript();

    void addFn(const char* name, guilefn func);
    
    double load(const char* filename);

private:
    SCM *guile;
};
