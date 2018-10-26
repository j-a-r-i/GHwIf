/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#pragma once

class Log
{
public:    
    static void err(const char* module, const char* reason);
    static void msg(const char* module, const char* msg);
    static void dbg(const char* module, const char* msg);

    static void value(const char* name, const char* value);
    static void value(const char* name, double      value);
};
