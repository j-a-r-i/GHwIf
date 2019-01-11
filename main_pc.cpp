#include "common.h"
#include "main_pc.h"
#include "logger.h"
#ifdef HW_LINUX
#  include <sys/socket.h>
#else
#  include <winsock2.h>
#  include <WS2tcpip.h>
#endif

void pc_init(BaseRuntime *rt)
{
    Log::value("HW", "PC");

#ifndef HW_LINUX
	WSADATA wsaData = { 0 };
	int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (res != 0) {
		Log::err("WSAStartup failed", res);
	}
#endif
}
