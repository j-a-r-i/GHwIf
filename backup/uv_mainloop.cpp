#include <uv.h>

uv_loop_t *loop;
uv_timer_t timer1;
uv_tcp_t *client;

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
