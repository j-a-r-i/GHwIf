#include <iostream>
#include "sensors.h"
#include "btooth.h"
#include "Socket.h"
#include "measures.h"
#include "db.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <uv.h>

//#pragma comment (lib, "libuv.lib")

#define PORT 8080
#define NUM_CONNECTIONS 10

uv_loop_t *loop;
uv_timer_t timer1;

//------------------------------------------------------------------------------
void onConnection(uv_stream_t* server, int status)
{
	std::cout << "Connection" << std::endl;
}

//------------------------------------------------------------------------------
void onRead()
{
}

//------------------------------------------------------------------------------
void onTimer(uv_timer_t* handle)
{
	std::cout << "timer1" << std::endl;
}

//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	uv_tcp_t server;
	sockaddr_in addr;
	int backlog = 0;

    loop = uv_default_loop();

	uv_timer_init(loop, &timer1);
	uv_timer_start(&timer1, onTimer, 5000, 5000);

	uv_tcp_init(loop, &server);

	uv_ip4_addr("0.0.0.0", PORT, &addr);

	uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);

	uv_listen((uv_stream_t*)&server, NUM_CONNECTIONS, onConnection);

	uv_run(loop, UV_RUN_DEFAULT);
}

//------------------------------------------------------------------------------
int test1(int argc, char *argv[])
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

	return 0;
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
