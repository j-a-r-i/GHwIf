/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#include "ssh.h"
#include "config.h"
#include "logger.h"
#include <unistd.h>


/** Constructor - create connection to remote host
 */
Ssh::Ssh(const char *host, const char *username, const char *password)
{
    int verbosity = SSH_LOG_PROTOCOL;
    int port = 22;
    int err;
    
    session = ssh_new();
    if (session == NULL)
	return;

    ssh_options_set(session, SSH_OPTIONS_HOST, host);
    ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
    ssh_options_set(session, SSH_OPTIONS_PORT, &port);

    err = ssh_connect(session);
    if (err != SSH_OK) {
	Log::err("Ssh.connect", ssh_get_error(session));
	return;
    }

    err = ssh_userauth_password(session, NULL, password);
    if (err != SSH_AUTH_SUCCESS) {
	Log::err("Ssh.password", ssh_get_error(session));
	return;
    }

    channel = ssh_channel_new(session);
    if (channel == NULL) {
	Log::err("Ssh", "channel create");
	return;
    }
}

/** Execute command in remote server
 */
void Ssh::execute(const char *command)
{
    char buffer[256];
    int err;
    int bytes;

    err = ssh_channel_open_session(channel);
    if (err != SSH_OK) {
	Log::err("Ssh.open", ssh_get_error(session));
	return;
    }
    
    err = ssh_channel_request_exec(channel, command);
    if (err != SSH_OK) {
	Log::err("Ssh.exec", ssh_get_error(session));
	return;
    }

    bytes = ssh_channel_read(channel, buffer, sizeof(buffer),  0);
    while (bytes > 0) {
	write(STDOUT_FILENO, buffer, bytes);

	bytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    }

    if (bytes >= 0)
	ssh_channel_send_eof(channel);
    else
	Log::err("Ssh.read", ssh_get_error(session));

    ssh_channel_close(channel);
}

/** Cleanup resources
 */
Ssh::~Ssh()
{
    if (channel != NULL)
	ssh_channel_free(channel);
    
    ssh_disconnect(session);
    if (session != NULL)
	ssh_free(session);
}

