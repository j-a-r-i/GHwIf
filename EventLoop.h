#pragma once

#include <uv.h>
#include <string>
#include "logger.h"
#include "common.h"

//-----------------------------------------------------------------------------
/** Defines the interface for event loops. See @UvEventLoop
 */
class EventLoop
{
public:
	virtual void run() = 0;
};

//-----------------------------------------------------------------------------
/** Asyncronous timer.
 */
class UvTimer
{
public:
	UvTimer(uint64_t duration) :
		timeout{duration}
	{
		timer.data = this;
	}

	uv_timer_t* getHandle() {
		return &timer;
	}

	uint64_t getTimeout() {
		return timeout;
	}

	void onTimeout();
private:
	uv_timer_t timer;
	uint64_t  timeout;
};

//-----------------------------------------------------------------------------
/** Read standard input asyncronously.
 */
class UvStdin
{
public:
	UvStdin(IPluginScript *scr) :
		script{scr}
	{
		uvHandle.data = this;
	}

	uv_tty_t* getHandle() {
		return &uvHandle;
	}

	void onData(std::string& str) {
		Log::msg("stdin", str.c_str());
		script->eval(str);
	}

	void onEof() {
		Log::msg("stdin", "<EOF>");
	}
private:
	uv_tty_t uvHandle;
	IPluginScript *script;
};

//-----------------------------------------------------------------------------
/** uv_poll might work only in linux with serial ports?
 */
class UvSerial
{
public:
	UvSerial(const char* port) :
		fd{0}
	{
	}

	int getHandle() {
		return fd;
	}
	uv_poll_t* getUvHandle() {
		return &uvHandle;
	}

private:
	int fd;
	uv_poll_t uvHandle;
};

//-----------------------------------------------------------------------------
class UvTcpServer
{
public:
	UvTcpServer() {
	}

	uv_tcp_t* getUvHandle() {
		return &socket;
	}

private:
	uv_tcp_t socket;
};

//-----------------------------------------------------------------------------
/** libuv based event loop. See https://libuv.org/
 */
class UvEventLoop : public EventLoop
{
public:
	UvEventLoop();
	~UvEventLoop();

	void add(UvTimer& timer);
	void add(UvStdin& pstdin);
	void add(UvSerial& handler);
	void add(UvTcpServer& server, const char* host, int port);

	void run() override;
private:
	uv_loop_t *loop;
	uv_timer_t timer1;
};

