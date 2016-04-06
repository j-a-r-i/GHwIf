#include <iostream>
#include "sensors.h"
#include "btooth.h"
#include "Socket.h"

#define PORT 8080


//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int loop = 1;
#if 0
    Sensors s;
    s.scan();
#endif

#if 0
    Bluetooth b;
    b.scan();
#endif    

    FileList  handles;
    SocketServer server(PORT, &handles);

    handles.add(&server);

    RS232  serial("/dev/ttyACM0");

    handles.add(&serial);

    
    while (loop) {
	fd_set reads;
	int maxFd = 0;
      
	//std::cout << "looping..." << std::endl;

	for (auto& i : handles._items) {
	    //std::cout << "\tH:" << i->_handle << std::endl;
	    FD_SET(i->_handle, &reads);
	    if (i->_handle > maxFd)
		maxFd = i->_handle;
	}
	
	select(maxFd+1, &reads, NULL, NULL, NULL);

	for (auto& i : handles._items) {
	    if (FD_ISSET(i->_handle, &reads)) {
		i->HandleSelect();
	    }
	}
    }
    
    return 0;
}
