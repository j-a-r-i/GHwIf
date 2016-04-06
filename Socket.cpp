#include <iostream>
#include <sys/socket.h>
#include "Socket.h"

#define TRUE 1

//------------------------------------------------------------------------------
SocketServer::SocketServer(int port, FileList *handles) :
  _handles(handles)
{
    int opt = TRUE;
    
    _handle = socket(AF_INET, SOCK_STREAM, 0);
    if (_handle == 0) {
        std::cout << "Can't open socket!" << std::endl;
    }

    if (setsockopt(_handle, SOL_SOCKET, SO_REUSEADDR,
                   (char*)&opt, sizeof(opt)) < 0) {
        std::cout << "Error setsockopt to reuseaddr" << std::endl;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(_handle, (sockaddr*)&address, sizeof(address)) < 0) {
        std::cout << "Error bind" << std::endl;
    }

    if (listen(_handle, 5) < 0) {
       std::cout << "Error listen" << std::endl;
    }
}

void SocketServer::HandleSelect()
{
    int handle;
    socklen_t len = sizeof(address);

    //std::cout << "HandleSelect" << std::endl;
    
    handle = accept(_handle, (struct sockaddr *)&address, &len);
    if (handle < 0) {
        std::cout << "Error accept" << std::endl;
    }

    Socket *s = new Socket(handle, _handles);

    _handles->add(s);
}

//------------------------------------------------------------------------------
Socket::Socket(int hndl, FileList *handles) :
  _handles(handles)
{
  _handle = hndl;
}

void Socket::HandleSelect()
{
    const int SIZE = 80;
    char buffer[SIZE];
    int count;
    //std::cout << "HandleSelect2" << std::endl;

    count = read(buffer, SIZE);
    if (count == 0) {
	std::cout << "connection closed " << _handle << std::endl;
	_handles->del(this);
    }
    else {
	std::cout << "S(" << count << "):" << buffer << std::endl;
    }
}
