#include "EventLoopSelect.h"
#include "logger.h"
#ifdef HW_LINUX
#  include <sys/socket.h>
#  include <sys/timerfd.h>
#else
#  include <winsock2.h>
#  include <WS2tcpip.h>
#endif

#ifdef HW_LINUX
//------------------------------------------------------------------------------
FileTimer::FileTimer(unsigned int sec) :
	FileBase("Timer")
{
	struct itimerspec ts;
	struct itimerspec *oldts;
	int err;

	_handle = timerfd_create(CLOCK_MONOTONIC, 0);
	if (_handle == -1) {
		Log::err("timerfd_create", errno);
		return;
	}

	ts.it_interval.tv_sec = sec;
	ts.it_interval.tv_nsec = 0;
	ts.it_value.tv_sec = sec;
	ts.it_value.tv_nsec = 0;

	err = timerfd_settime(_handle, 0, &ts, NULL);
	if (err < 0) {
		Log::value("handle", _handle);
		Log::err("timerfd_settime", errno);
		_handle = HANDLE_ERROR;
		return;
	}
}

void FileTimer::HandleSelect()
{
	uint64_t res;

	Log::msg("FileTimer", "select");

	read(&res);
}
#endif


//------------------------------------------------------------------------------
SocketServer::SocketServer(int port, EventLoopSelect *loop) :
	FileBase("SServer"),
	_loop(loop)
{
	int opt = TRUE;

	_handle = socket(AF_INET, SOCK_STREAM, 0);
	if (_handle == 0) {
		Log::err(__FUNCTION__, "open socket");
	}

	if (setsockopt(_handle, SOL_SOCKET, SO_REUSEADDR,
		(char*)&opt, sizeof(opt)) < 0)
	
	{
		Log::err(__FUNCTION__, "setsockopt to reuseaddr");
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if (bind(_handle, (sockaddr*)&address, sizeof(address)) < 0) {
		Log::err(__FUNCTION__, "bind");
	}

	if (listen(_handle, 5) < 0) {
		Log::err(__FUNCTION__, "listen");
	}
}

void SocketServer::HandleSelect()
{
	int handle;
	socklen_t len = sizeof(address);

	//std::cout << "HandleSelect" << std::endl;

	handle = accept(_handle, (struct sockaddr *)&address, &len);
	if (handle < 0) {
		Log::err(__FUNCTION__, "accept");
	}

	Socket *s = new Socket(handle);

	_loop->add(s);
}

//------------------------------------------------------------------------------
Socket::Socket(int hndl) :
	FileBase("socket")
{
	_handle = hndl;
}

void Socket::HandleSelect()
{
	const int SIZE = 80;
	char buffer[SIZE];
	int count;

	count = read(buffer, SIZE);
	if (count == 0) {
		Log::msg(__FUNCTION__, "socket: connection closed");
		deleted = true;
	}
	else {
		Log::msg("socket read", buffer);
	}
}


//------------------------------------------------------------------------------
EventLoopSelect::EventLoopSelect()
{
}


//------------------------------------------------------------------------------
EventLoopSelect::~EventLoopSelect()
{
}

void EventLoopSelect::run()
{
	// main loop
	//
	int loopCount = 5;
	while (loopCount) {
		fd_set reads;
		int maxFd = 0;

		FD_ZERO(&reads);
		for (auto& i : _items) {
			if ((i->getHandle() == HANDLE_ERROR) ||
				i->isDeleted())
				continue;
			//i->dump();
			FD_SET(i->_handle, &reads);
			if (i->getHandle() > maxFd)
				maxFd = i->_handle;
		}

		select(maxFd + 1, &reads, NULL, NULL, NULL);

		for (auto& i : _items) {
			if (FD_ISSET(i->_handle, &reads)) {
				i->HandleSelect();
			}
		}
		//loopCount--;
	}
}
