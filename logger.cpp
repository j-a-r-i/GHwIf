/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#include "logger.h"
#include <iostream>
#include <iomanip>

void Log::err(const char* module, const char* reason)
{
    std::cout << "ERROR " << module << ": " << reason << std::endl;
}

void Log::err(const char* module, int code)
{
    std::cout << "ERROR " << module << ": " << code << std::endl;
}

void Log::msg(const char* module, const char* msg)
{
    std::cout << "      " << module << ": " << msg << std::endl;
}

void Log::msg(const char* module, int val)
{
    std::cout << "      " << module << ": " << val << std::endl;
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
    std::cout << "    V:" << name << " = "
	      << std::setprecision(8)
	      << value << std::endl;    
}
