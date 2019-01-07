#pragma once

#include <uv.h>

class EventLoop
{
public:
	EventLoop();
	~EventLoop();

	virtual void run() = 0;
};

class UvTimer
{
public:
	UvTimer() {
		timer.data = this;
	}

	uv_timer_t* getTimer() {
		return &timer;
	}

	void event();
private:
	uv_timer_t timer;
};

class UvEventLoop : public EventLoop
{
public:
	UvEventLoop();
	~UvEventLoop();

	void add(UvTimer& timer, uint64_t duration);

	void run() override;
private:
	uv_loop_t *loop;
	uv_timer_t timer1;
};