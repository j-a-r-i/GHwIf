/******************************************************************************
 * Copyright (C) 2016-8 Jari Ojanen
 ******************************************************************************/
#include <iostream>
#include <fstream>
#include <list>
#include <unistd.h>
#include "config.h"
#include "logger.h"
#include "Socket.h"
#include "measures.h"
//#include "db.h"
#include "web.h"
#include "disk.h"
#include "sun.h"
#include "scmscript.h"
#include "common.h"
#ifdef HW_RPI
  #include "rpi/main_rpi.h"
#else
  #include "main_pc.h"
#endif

#ifdef USE_JSON
  #include "rapidjson/document.h"
  #include "rapidjson/writer.h"
  #include "rapidjson/stringbuffer.h"
#endif
#ifdef USE_BLUETOOTH
  #include "btooth.h"
#endif
#ifdef USE_SENSORS
  #include "sensors.h"
#endif

#include <uv.h>

#ifdef USE_WS
  #define _WEBSOCKETPP_CPP11_THREAD_
  #include <websocketpp/config/core.hpp>
  #include <websocketpp/server.hpp>
#endif

//#pragma comment (lib, "libuv.lib")

#define PORT 8080
#define NUM_CONNECTIONS 10

uv_loop_t *loop;
uv_timer_t timer1;
uv_tcp_t *client;
Measure  meas;
RS232  serial(SERIAL_PORT, &meas);

#ifdef USE_WS
typedef websocketpp::server<websocketpp::config::core> server;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
#endif

//------------------------------------------------------------------------------
void onClose(uv_handle_t* handle) 
{
	std::cout << "Connection closed" << std::endl;

	free(client);
}


//------------------------------------------------------------------------------
void onRead(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
{
	if (nread < 0) {
		return;
	}
	buf->base[nread] = 0;

	std::cout << "READ:" << nread << " - " << buf->base << std::endl;

	//free(buf->base);
}

void alloc_buffer(uv_handle_t* handle, size_t ssize, uv_buf_t* buf)
{
	*buf = uv_buf_init((char*)malloc(ssize), ssize);
}


//------------------------------------------------------------------------------
void onConnection(uv_stream_t* server, int status)
{
	int res;

	std::cout << "Connection" << std::endl;
	if (status < 0) {
		std::cout << "error in connection " << status << std::endl;
		uv_close((uv_handle_t*)client, onClose);
		return;
	}

	client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));

	uv_tcp_init(loop, client);
	
	res = uv_accept(server, (uv_stream_t*)client);
	if (res == 0) {
		uv_read_start((uv_stream_t*)client, alloc_buffer, onRead);
	}
	else {
		uv_close((uv_handle_t*)client, onClose);
	}
}

//------------------------------------------------------------------------------
void onTimer(uv_timer_t* handle)
{
	std::cout << "timer1" << std::endl;
}

#ifdef USE_WS
//------------------------------------------------------------------------------
void onMessage(server* s, websocketpp::connection_hdl hdl, server::message_ptr msg)
{
    if (msg->get_opcode() == websocketpp::frame::opcode::text) {
        s->get_alog().write(websocketpp::log::alevel::app,
                    "Text Message Received: "+msg->get_payload());
    }
	else {
        s->get_alog().write(websocketpp::log::alevel::app,
                    "Binary Message Received: "+websocketpp::utility::to_hex(msg->get_payload()));
    }

    try {
        s->send(hdl, msg->get_payload(), msg->get_opcode());
    }
	catch (const websocketpp::lib::error_code& e) {
        s->get_alog().write(websocketpp::log::alevel::app,
                    "Echo Failed: "+e.message());
    }
}
#endif

//------------------------------------------------------------------------------
void onRs232Message(uv_poll_t *req, int status, int events)
{
    if (status < 0) {
	std::cout << "RS232 error " << status << std::endl;
    }
    else if (events & UV_READABLE) {
    }
}

//------------------------------------------------------------------------------
int test1(int argc, char *argv[])
{
	uv_tcp_t server;
	uv_poll_t uvSerial;
	sockaddr_in addr;
	//int backlog = 0;

    loop = uv_default_loop();

	uv_timer_init(loop, &timer1);
	uv_timer_start(&timer1, onTimer, 5000, 5000);

	uv_tcp_init(loop, &server);

	uv_ip4_addr("0.0.0.0", PORT, &addr);

	uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);

	uv_listen((uv_stream_t*)&server, NUM_CONNECTIONS, onConnection);

	uv_poll_init(loop, &uvSerial, serial.GetHandle());
	uv_poll_start(&uvSerial, UV_READABLE, onRs232Message);

	uv_run(loop, UV_RUN_DEFAULT);

	return 0;
}

#ifdef USE_JSON
//------------------------------------------------------------------------------
int test2(int argc, char *argv[])
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
#endif

#ifdef USE_WS
void test3()
{
	server s;
	std::ofstream log;

	try {
		s.set_access_channels(websocketpp::log::alevel::all);

		// Log to a file rather than stdout, as we are using stdout for real
		// output
		//log.open("output.log");
		//s.get_alog().set_ostream(&log);
		//s.get_elog().set_ostream(&log);

		// print all output to stdout
		//s.register_ostream(&std::cout);

		//s.init

		// Register our message handler
		s.set_message_handler(bind(&onMessage, &s, ::_1, ::_2));

		server::connection_ptr con = s.get_connection();

		con->start();

		char a;
		while (std::cin.get(a)) {
			con->read_some(&a, 1);
		}
		con->eof();
	}
	catch (websocketpp::exception const & e) {
		std::cout << e.what() << std::endl;
	}
	log.close();

}
#endif

void test4()
{
#ifdef USE_BLUETOOTH
    Bluetooth b;

    b.scanStart();
    b.scanLoop();
    b.scanStop();

    b.close();
#endif
}

const char* errorString[] = {
    "missing argument",
    "argument type"
};

//------------------------------------------------------------------------------
const char* TheException::what() const throw () {
    return errorString[error];
}


//------------------------------------------------------------------------------
class Runtime : public BaseRuntime
{
public:
    void add(InfoReader* reader) {
	readers.push_back(reader);
    }

    void exec() {
	scm.load("test.scm");
	scm.mainLoop();
    }

    void addFunc(const char* name, foreign_func func) {
	scm.addFn(name, func);
    }

    void readAll() {
	for (auto reader : readers) {
	    reader->read();
	    reader->print();
	}
    }

    void dump() {
	for (auto reader : readers) {
	    reader->dump();
	}
    }
    
    void webLoad(int i, int arg) {
	web.setSite((Web::Site)i, arg);
	web.read();
    }

    void webVerbose(bool value) {
	web.setVerbose(value);
    }
    
private:
    ScmScript scm;
    std::list<InfoReader*> readers;
    Web web;
};

Runtime gRuntime;

int arg_integer(scheme *sch, pointer arg)
{
    int retVal = 0;
    
    if (arg != sch->NIL) {
	pointer car = pair_car(arg);
	if (is_integer(car)) {
	    retVal = ivalue(car);
	}
	else {
	    throw TheException(TheException::EArgumentType);
	}
    }
    else {
	throw TheException(TheException::EMissingArgument);
    }
    return retVal;
}

pointer scm_web_load(scheme *sch, pointer args)
{
    try {
	int i1 = arg_integer(sch, args);
	int i2 = arg_integer(sch, pair_cdr(args));

	gRuntime.webLoad(i1, i2);
    }
    catch (TheException& e) {
	Log::err(__FUNCTION__, e.what());
    }
    return sch->NIL;
}

pointer scm_web_verbose(scheme *sch, pointer args)
{
    try {
	int i = arg_integer(sch, args);

	gRuntime.webVerbose(i);
    }
    catch (TheException& e) {
	Log::err(__FUNCTION__, e.what());
    }
    return sch->NIL;
}

pointer scm_read_all(scheme *sch, pointer args)
{
    if (args == sch->NIL) {
	gRuntime.readAll();
    }
    else {
	Log::err(__FUNCTION__, "extra argument");
    }
    return sch->NIL;
}

pointer scm_dump(scheme *sch, pointer args)
{
    if (args == sch->NIL) {
	gRuntime.dump();
    }
    else {
	Log::err(__FUNCTION__, "extra argument");
    }
    return sch->NIL;
}



//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
#ifdef HW_RPI
    rpi_init(&gRuntime);
#else
    pc_init(&gRuntime);
#endif

    gRuntime.addFunc("web-load",    scm_web_load);
    gRuntime.addFunc("web-verbose", scm_web_verbose);
    gRuntime.addFunc("read-all",    scm_read_all);
    gRuntime.addFunc("dump",        scm_dump);
    
#ifdef USE_SENSORS
    Sensors s;
    gRuntime.add(&s);
#endif

    Web w;
    //gRuntime.add(&w);

    Disk d;
    gRuntime.add(&d);

    Sun sun;
    gRuntime.add(&sun);
    
    gRuntime.webLoad(0, 24311);
    
    gRuntime.exec();

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
