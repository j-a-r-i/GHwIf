/******************************************************************************
 * Copyright (C) 2016-8 Jari Ojanen
 ******************************************************************************/
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include "common.h"
#ifdef HW_LINUX
#include <unistd.h>
#endif
#include "logger.h"
#include "EventLoopSelect.h"
#include "file.h"
#include "db.h"
#include "web.h"
#include "disk.h"
#include "sun.h"
#include "EventLoop.h"
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
#ifdef USE_WS
  #define _WEBSOCKETPP_CPP11_THREAD_
  #include <websocketpp/config/core.hpp>
  #include <websocketpp/server.hpp>
#endif

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
    "argument type",
    "tidy: parse buffer",
    "tidy: clean and repair",
    "tidy: run diagnostics",
    "missing runtime"
};

//------------------------------------------------------------------------------
const char* TheException::what() const throw () {
    return errorString[error];
}

//------------------------------------------------------------------------------
class MyScheduler : public IScheduler
{
public:
	void add(ISchedulerEvent *event) {
		items.push_back(std::unique_ptr<ISchedulerEvent>(event));
	}
	void elapsed() {
	}

private:
	std::vector<std::unique_ptr<ISchedulerEvent>> items;
};

//------------------------------------------------------------------------------
class Runtime : public IRuntime
{
public:
    Runtime() :
	db(Cfg::get(CfgItem::SQLITE_DB))
    {
    }
    
    void dbQuery(const char* sql) {
	Query query(&db, sql);

	query.Handle();
    }

    void version() {
	web.version();
	db.version();
    }
private:
    Web web;
    Database db;
};


//------------------------------------------------------------------------------
// initialize the system
//
void init(Script* script)
{
#ifdef SCR_SCHEME
    scm_func_init(script);
#endif

#ifdef HW_RPI
    rpi_init(script);
#else
    pc_init(script);
#endif

    script->load(Cfg::get(CfgItem::SCRIPT_FILE));
}

//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	Cfg::init();  // configuration paramerers must be initialized first
	Runtime rt;
	Script script(&rt);
	UvEventLoop loop;
	UvTimer timer1(5000);
	UvStdin stdin1(&script);
	UvTcpServer server;

	init(&script);
	rt.version();
	script.version();

	script.exec("OnTimer");

	loop.add(timer1);
	loop.add(stdin1);
	loop.add(server, "0.0.0.0", 5005);

	loop.run();
}

//------------------------------------------------------------------------------
int main_old(int argc, char *argv[])
{
	Cfg::init();  // configuration paramerers must be initialized first
	MyScheduler scheduler;
	Runtime rt;
	Script script(&rt);

       init(&script);
    
#ifdef USE_SENSORS
	scheduler.add(new Sensors(script));
#endif
#ifdef USE_BLUETOOTH
    Bluetooth b;
    b.scan();
#endif    

	scheduler.add(new Sun(script));

#ifdef HW_LINUX
	scheduler.add(new Disk(script));
#endif
 
    // Init 'file' handles
    //
	EventLoopSelect loop;
    SocketServer server(PORT, &loop);
    loop.add(&server);

#ifdef HW_LINUX
    FileStdin fstdin;
    loop.add(&fstdin);
    // add callback rt.scr_eval(line);

//    FileNotify fnotify(INIT_SCRIPT);
//    loop.add(&fnotify);

    FileSignal fsignal;
    loop.add(&fsignal);

    FileTimer timer1(5);
    loop.add(&timer1);
    // add callback rt.scr_run("timer");
#endif

    RS232  serial(script, STR_SERIAL_PORT);
    loop.add(&serial);

    //rt.add(&serial);

	loop.run();

	return 0;
}
