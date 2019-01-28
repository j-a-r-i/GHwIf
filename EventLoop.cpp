#include "EventLoop.h"
//#include <iostream>
#include "logger.h"

//------------------------------------------------------------------------------
UvEventLoop::UvEventLoop()
{
	loop = uv_default_loop();

	Log::msg("Mem free ", (int)(uv_get_free_memory() / (1024L * 1024L)));
	Log::msg("Mem total", (int)(uv_get_total_memory() / (1024L * 1024L)));
}

UvEventLoop::~UvEventLoop()
{
}

//------------------------------------------------------------------------------
void onTimer(uv_timer_t* handle)
{
	UvTimer *timer = static_cast<UvTimer*>(handle->data);
	timer->onTimeout();
}

void UvEventLoop::add(UvTimer& timer)
{
	// Note: no error handling because these functions nearly always return zero.
	//
	uv_timer_init(loop, timer.getHandle());
	uv_timer_start(timer.getHandle(), onTimer, timer.getTimeout(), timer.getTimeout());
}

//------------------------------------------------------------------------------
void alloc_buffer(uv_handle_t *handle, size_t size, uv_buf_t *buf)
{
	*buf = uv_buf_init((char*)malloc(size), size);
}

void read_stdin(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) 
{
	UvStdin *sin = static_cast<UvStdin*>(stream->data);

	if (nread < 0) {
		if (nread == UV_EOF) {
			sin->onEof();
		}
	}
	else if (nread > 0) {
		std::string buffer(buf->base, nread);
		sin->onData(buffer);
	}

	// OK to free buffer as write_data copies it.
	if (buf->base)
		free(buf->base);
}
void UvEventLoop::add(UvStdin& sin)
{
	constexpr int STDIN_HANDLE = 0;
	int err;
	err = uv_tty_init(loop, sin.getHandle(), STDIN_HANDLE, 0);   // todo check 0 value!
	if (err != 0) {
		Log::err("uv_tty_init", err);
		return;
	}

	/*
	err = uv_pipe_open(sin.getHandle(), 0);
	if (err != 0) {
		Log::err("uv_pipe_open", err);
		return;
	}*/

	err = uv_read_start((uv_stream_t*)sin.getHandle(), alloc_buffer, read_stdin);
	if (err != 0) {
		Log::err("uv_tty_read", err);
		return;
	}
}

//------------------------------------------------------------------------------
void onSerialRead(uv_poll_t *req, int status, int events)
{

}

void UvEventLoop::add(UvSerial & handler)
{
	int err;
	err = uv_poll_init(loop, handler.getUvHandle(), handler.getHandle());
	if (err) {
		Log::err("uv_poll_init", err);
		return;
	}
	
	err = uv_poll_start(handler.getUvHandle(), UV_READABLE, onSerialRead);
	if (err) {
		Log::err("uv_poll_init", err);
		return;
	}
}

//------------------------------------------------------------------------------
void UvEventLoop::run()
{
	uv_run(loop, UV_RUN_DEFAULT);
}

void UvTimer::onTimeout()
{
    //Log::msg("timer", "timeout");
}
