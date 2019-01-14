/******************************************************************************
 * Copyright (C) 2016-8 Jari Ojanen
 ******************************************************************************/
#include <iostream>
#include <fstream>
#include <list>
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
class Runtime : public IRuntime
{
public:
    Runtime() :
	db(Cfg::get(CfgItem::SQLITE_DB))
    {
    }
    
    void add(InfoReader* reader) {
	readers.push_back(reader);
    }

    void readAll() {
	for (auto& reader : readers) {
	    reader->read();
	    reader->print();
	}
    }

    void dump() {
	for (auto& reader : readers) {
	    reader->dump();
	}
    }
    
    void webLoad(int i, int arg) {
	web.setSite((Web::Site)i, arg);
	web.read();
    }

    void webGet(const char* url) {
	web.setSite(url, "a");
	web.read();
    }

    void webVerbose(bool value) {
	web.setVerbose(value);
    }

    void dbQuery(const char* sql) {
	Query query(&db, sql);

	query.Handle();
    }
    
private:
    std::list<InfoReader*> readers;
    Web web;
    Database db;
};

//------------------------------------------------------------------------------
IRuntime *gRuntime;

//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	Cfg::init();  // configuration paramerers must be initialized first

	Script script;
	Runtime rt;
	UvEventLoop loop;
	UvTimer timer1(5000);
	UvStdin stdin1(&script);


	pc_init(&script);

	loop.add(timer1);
	loop.add(stdin1);

	loop.run();
}

//------------------------------------------------------------------------------
int main_old(int argc, char *argv[])
{
    Script script;
    Runtime rt;

    gRuntime = &rt;

    // init readers
    //
#ifdef HW_RPI
    rpi_init(gRuntime);
#else
    pc_init(&script);
#endif

#ifdef SCR_SCHEME
    scm_func_init(&script);
#endif
    
#ifdef USE_SENSORS
    Sensors s;
    gRuntime->add(&s);
#endif
#ifdef USE_BLUETOOTH
    Bluetooth b;
    b.scan();
#endif    

    Sun sun;
    gRuntime->add(&sun);
    sun.read();

#ifdef HW_LINUX
    Disk d;
    gRuntime->add(&d);

    // parse arguments
    //
    bool scriptLoaded = false;
    int  opt;
    while ((opt = getopt(argc, argv, "f:s:")) != -1) {
	switch (opt) {
	case 'f':
	    rt.script.load(optarg);
	    scriptLoaded = true;
	    break;
	case 's':
	    Log::msg("set serial port", optarg);
	    break;
	default:
	    Log::msg("usage", "hwif -f filename -s serial_port");
	    exit(1);
	}
    }

    if (!scriptLoaded) { // use the default script
	rt.script.load(INIT_SCRIPT);
    }
#endif  
    // Init 'file' handles
    //
	EventLoopSelect loop;
    SocketServer server(PORT, &loop);
    loop.add(&server);

#ifdef HW_LINUX
    FileStdin fstdin;
    loop.add(&fstdin);
    // add callback gRuntime->scr_eval(line);

    FileNotify fnotify(INIT_SCRIPT);
    loop.add(&fnotify);

    FileSignal fsignal;
    loop.add(&fsignal);

    FileTimer timer1(5);
    loop.add(&timer1);
    // add callback gRuntime->scr_run("timer");
#endif

    RS232  serial(STR_SERIAL_PORT);
    loop.add(&serial);

    gRuntime->add(&serial);

	loop.run();

	return 0;
}
