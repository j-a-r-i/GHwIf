/******************************************************************************
 * Copyright (C) 2019 Jari Ojanen
 ******************************************************************************/
#pragma once

#include <libssh/libssh.h>

class Ssh
{
public:
    Ssh(const char *host, const char *username, const char *password);
    ~Ssh();

    void execute(const char *command);
    
private:
    ssh_session session;
    ssh_channel channel;
};
