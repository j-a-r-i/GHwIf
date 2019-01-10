#pragma once

#include <uv.h>
#include <string>
#include "logger.h"

//-----------------------------------------------------------------------------
/** Defines the interface for event loops. See @UvEventLoop
 */
class EventLoop
{
public:
	EventLoop();
	~EventLoop();

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
 *  Does not work in windows!
 */
class UvStdin
{
public:
	UvStdin() {
		uvHandle.data = this;
	}

	uv_pipe_t* getHandle() {
		return &uvHandle;
	}

	void onData(std::string str) {
		Log::msg("stdin", str.c_str());
	}

	void onEof() {
		Log::msg("stdin", "<EOF>");
	}
private:
	uv_pipe_t uvHandle;
};

//-----------------------------------------------------------------------------
/** uv_poll might work only in linux with serial ports?
 */
class UvSerial
{
public:
	UvSerial(const char* port) {

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

	void run() override;
private:
	uv_loop_t *loop;
	uv_timer_t timer1;
};