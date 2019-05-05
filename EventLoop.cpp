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
void onConnection(uv_stream_t* server, int status)
{
	Log::msg("socket", "connection");
	if (status < 0) {
		Log::err("socket connection", status);
		///uv_close((uv_handle_t*)client, onClose);
		return;
	}

	UvTcpServer* uvSrv = static_cast<UvTcpServer*>(server->data);
	uvSrv->onConnection();
	/*
	client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));

	uv_tcp_init(loop, client);

	res = uv_accept(server, (uv_stream_t*)client);
	if (res == 0) {
		uv_read_start((uv_stream_t*)client, alloc_buffer, onRead);
	}
	else {
		uv_close((uv_handle_t*)client, onClose);
	}
	*/
}

//------------------------------------------------------------------------------
void onRead(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
{
	if (nread < 0) {
		Log::err("onRead", nread);
		return;
	}
	buf->base[nread] = 0;

	Log::msg("onRead", buf->base);

	UvTcpServer* uvSrv = static_cast<UvTcpServer*>(stream->data);
	uvSrv->onRead();

	free(buf->base);
}

void onClose(uv_handle_t* handle)
{
	Log::msg("socket", "close");

	//free(client);
}

void onWrite(uv_write_t* req, int status) 
{
	Log::msg("onWrite", status);

	uv_close((uv_handle_t*)req->handle, onClose);
}
//------------------------------------------------------------------------------
const char* RESPONSE =
"HTTP/1.1 200 OK\r\n" \
"Content-Type: text/plain\r\n" \
"Content-Length: 12\r\n" \
"\r\n" \
"hello world\n";



void UvTcpServer::onConnection()
{
	int err;

	if (clientConnected) {
		Log::err("UvTcpServer", "only one client allowed!");
		return;
	}

	clientConnected = true;

	err = uv_tcp_init(socket.loop, &client);
	if (err) {
		Log::err("uv_tcp_init (client)", err);
		return;
	}
	client.data = this;

	err = uv_accept((uv_stream_t*)&socket, (uv_stream_t*)&client);
	if (err == 0) {
		uv_read_start((uv_stream_t*)&client, alloc_buffer, ::onRead);
	}
	else {
		Log::err("uv_accept", err);
		uv_close((uv_handle_t*)&client, ::onClose);
	}
}

void UvTcpServer::onRead()
{
	Log::err("server::onRead", 0);
	//uv_close((uv_handle_t*)&client, onClose);
	//clientConnected = false;

	static uv_buf_t uvbuf;

	uvbuf.base = const_cast<char*>(RESPONSE);
	uvbuf.len = sizeof(RESPONSE);

	uv_write(&write, (uv_stream_t*)& client, &uvbuf, 1, onWrite);
}

//------------------------------------------------------------------------------
void UvEventLoop::add(UvTcpServer& server, const char* host, int port)
{
	int err;
	sockaddr_in addr;

	err = uv_tcp_init(loop, server.getUvHandle());
	if (err) {
		Log::err("uv_tcp_init (server)", err);
		return;
	}
/*	err = uv_tcp_init(loop, server.getClientHandle());
	if (err) {
		Log::err("uv_tcp_init (client)", err);
		return;
	}*/
	err = uv_ip4_addr(host, port, &addr);
	if (err) {
		Log::err("uv_ip4_addr", err);
		return;
	}

	err = uv_tcp_bind(server.getUvHandle(), (const struct sockaddr*) & addr, 0);
	if (err) {
		Log::err("uv_tcp_bind", err);
		return;
	}

	err = uv_listen((uv_stream_t*)server.getUvHandle(), NUM_CONNECTIONS, onConnection);
	if (err) {
		Log::err("uv_tcp_bind", err);
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
