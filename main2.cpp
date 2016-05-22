#include <iostream>
#include "sensors.h"
#include "btooth.h"
#include "Socket.h"
#include "measures.h"
#include "db.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"


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
	rapidjson::Document doc;
	doc.SetObject();

	doc.AddMember("foo", 1, doc.GetAllocator());
	doc.AddMember("bar", 1.1, doc.GetAllocator());
	doc.AddMember("txt", "hello", doc.GetAllocator());

	rapidjson::StringBuffer strbuf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	doc.Accept(writer);

	std::cout << strbuf.GetString() << std::endl;


	Database db(":memory:");

	Query q1(&db, "CREATE TABLE test(t1 REAL, t2 REAL);");
	q1.Handle();

	Query q2(&db, "INSERT INTO test VALUES (10.1, 20.2);");
	q2.Handle();

	Query q3(&db, "INSERT INTO test VALUES (11.1, 22.2);");
	q3.Handle();

	Query q4(&db, "SELECT * FROM test;");
	q4.Handle();

}

//------------------------------------------------------------------------------
/*int main(int argc, char *argv[])
{
    int loop = 1;
    Measure meas;

#ifndef USE_LINUX
	WSADATA wsaData = { 0 };
	int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (res != 0) {
		std::cout << "WSAStartup failed: " << res << std::endl;
		return 1;
    }
#endif

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

#ifdef USE_LINUX
    RS232  serial("/dev/ttyACM0", &meas);

    handles.add(&serial);
#endif
    
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
*/