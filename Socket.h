#ifndef __SOCKET_H
#define __SOCKET_H

#include <netinet/in.h>
#include "serial.h"

//------------------------------------------------------------------------------
class SocketServer : public FileBase
{
public:
    SocketServer(int port, FileList *handles);

    void HandleSelect();

    sockaddr_in address;

private:
    FileList* _handles;
};

//------------------------------------------------------------------------------
class Socket : public FileBase
{
public:
    Socket(int hndl, FileList *handles);

    void HandleSelect();
    
private:
    FileList* _handles;
};


#endif //__SOCKET_H
