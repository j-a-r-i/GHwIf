/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#pragma once

#include <curl/curl.h>
#include "infoitem.h"

class Web : public InfoItem
{
public:
    Web();

    virtual ~Web();

    void read();
    void print();

    void onData(const char* str);

private:
    CURL *handle;
};
