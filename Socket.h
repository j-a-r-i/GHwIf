#ifndef __SOCKET_H
#define __SOCKET_H

#ifdef HW_LINUX
  #include <netinet/in.h>
#else
  #pragma comment(lib,"Ws2_32.lib")
  #include <WinSock2.h>
#endif
#include "serial.h"

//------------------------------------------------------------------------------
class SocketServer : public FileBase
{
public:
    SocketServer(int port, FileList *handles);

    void HandleSelect() override;

    sockaddr_in address;

private:
    FileList* _handles;
};

//------------------------------------------------------------------------------
class Socket : public FileBase
{
public:
    Socket(int hndl, FileList *handles);

    void HandleSelect() override;
    
private:
    FileList* _handles;
};


#endif //__SOCKET_H
