/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#include "logger.h"
#include <iostream>

void Log::err(const char* module, const char* reason)
{
    std::cout << "ERROR " << module << ": " << reason << std::endl;
}

void Log::msg(const char* module, const char* msg)
{
    std::cout << "      " << module << ": " << msg << std::endl;
}

void Log::dbg(const char* module, const char* msg)
{
    //std::cout << "      " << module << ": " << msg << std::endl;
}

void Log::value(const char* name, const char* value)
{
    std::cout << "    V:" << name << " = " << value << std::endl;
}

void Log::value(const char* name, double value)
{
    std::cout << "    V:" << name << " = " << value << std::endl;    
}
