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
#ifdef SCHEME
  #include "scmscript.h"
#else
  #include "luascript.h"
#endif

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

#include <glib.h>
#include <gio/gio.h>

#ifdef USE_WS
  #define _WEBSOCKETPP_CPP11_THREAD_
  #include <websocketpp/config/core.hpp>
  #include <websocketpp/server.hpp>
#endif

//#pragma comment (lib, "libuv.lib")

#define PORT 8080
#define NUM_CONNECTIONS 10

Measure  meas;
RS232  serial(SERIAL_PORT, &meas);

#ifdef USE_WS
typedef websocketpp::server<websocketpp::config::core> server;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
#endif


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

    void scr_init() {
	script.load(INIT_SCRIPT);
        //script.mainLoop();
    }

    void scr_run(const char* func) {
	script.exec(func);
    }

    void scr_eval(const char* line) {
	script.eval(line);
    }

    void addFunc(const char* name, foreign_func func) {
	script.addFn(name, func);
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
    Script script;
    std::list<InfoReader*> readers;
    Web web;
};

Runtime gRuntime;

#ifdef SCHEME
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
#endif

//------------------------------------------------------------------------------
GMainLoop *gLoop;


gboolean OnTimer(gpointer data)
{
    //g_print("timer\n");

    gRuntime.scr_run("timer");
    return TRUE;
}

gboolean OnStdIn(GIOChannel *io, GIOCondition cond, gpointer data)
{
#define LINE_LENGTH 80
    gsize line_length = LINE_LENGTH;
    gsize terminator_pos;
    gchar *line;
    gboolean ret = TRUE;
    GError *err = NULL;
    GIOStatus stat = g_io_channel_read_line(io,
					    &line,
					    &line_length,
					    &terminator_pos,
					    &err);
 
    switch (stat) {
    case G_IO_STATUS_ERROR:
	g_print("stdin: ERROR\n");
	ret = FALSE;
	break;
    case G_IO_STATUS_NORMAL:
	g_print("stdin: %s\n", line);
	gRuntime.scr_eval(line);
	g_free(line);
	break;
    case G_IO_STATUS_EOF:
	g_print("stdin: eof\n");
	g_main_loop_quit(gLoop);
	break;
    case G_IO_STATUS_AGAIN:
	g_print("stdin: again\n");
	break;
    default:
	g_print("stdin: invalid value\n");
	ret = FALSE;
	break;
    }
    return ret;
}

//------------------------------------------------------------------------------
void init_script()
{
#ifdef SCHEME
    gRuntime.addFunc("web-load",    scm_web_load);
    gRuntime.addFunc("web-verbose", scm_web_verbose);
    gRuntime.addFunc("read-all",    scm_read_all);
    gRuntime.addFunc("dump",        scm_dump);
#endif
}

//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    GError * err = NULL;
    
#ifdef HW_RPI
    rpi_init(&gRuntime);
#else
    pc_init(&gRuntime);
#endif

    init_script();
    
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
    
    gRuntime.scr_init();

    // mainloop and timer
    //
    gLoop = g_main_loop_new ( NULL , FALSE );

    GIOChannel *io = g_io_channel_unix_new(STDIN_FILENO);
    guint watchid =  g_io_add_watch(io, G_IO_IN, OnStdIn, NULL);

    g_timeout_add (5000 , OnTimer , gLoop); 

    g_main_loop_run (gLoop);

    g_io_channel_unref(io);
    g_main_loop_unref(gLoop);
    
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
