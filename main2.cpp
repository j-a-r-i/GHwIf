#include <iostream>
#include "sensors.h"
#include "btooth.h"
#include "Socket.h"
#include "measures.h"

#define PORT 8080

/*int main(int argc, char *argv[])
{
    std::string line = "m 3B 167 16C 0";
    std::istringstream sstr(line);

    char command;
    int  counter;
    int  temp1;
    int  temp2;
    int  temp3;

    sstr >> command >> std::hex >> counter >> temp1 >> temp2 >> temp3;

    float t1 = temp1 / 16.0;
    float t2 = temp2 / 16.0;
    //float t3 = temp3 / 16.0;
    
    std::cout << counter << std::endl;
    std::cout << t1 << std::endl;
    std::cout << t2 << std::endl;

    return 0;
}*/

//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int loop = 1;
    Measure meas;
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

    RS232  serial("/dev/ttyACM0", &meas);

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
