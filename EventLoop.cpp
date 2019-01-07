#include "EventLoop.h"
#include <iostream>


EventLoop::EventLoop()
{
}


EventLoop::~EventLoop()
{
}

UvEventLoop::UvEventLoop()
{
	loop = uv_default_loop();

	std::cout << "memory free " << uv_get_free_memory()/(1024L*1024L) << "M of " 
		                    << uv_get_total_memory()/(1024L*1024L) << "M" << std::endl;
}

UvEventLoop::~UvEventLoop()
{
}

//------------------------------------------------------------------------------
void onTimer(uv_timer_t* handle)
{
	std::cout << "timer1" << std::endl;

	UvTimer *timer = static_cast<UvTimer*>(handle->data);
	timer->event();
}

void UvEventLoop::add(UvTimer& timer, uint64_t duration)
{
	uv_timer_init(loop, timer.getTimer());
	uv_timer_start(timer.getTimer(), onTimer, duration, duration);
}

void UvEventLoop::run()
{
	uv_run(loop, UV_RUN_DEFAULT);
}

void UvTimer::event()
{
	std::cout << "Timer event" << std::endl;
}
