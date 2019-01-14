#include "common.h"
#include "main_pc.h"
#include "logger.h"
#ifdef HW_LINUX
#  include <sys/socket.h>
#else
#  include <winsock2.h>
#  include <WS2tcpip.h>
#endif

/** initialize PC hardware.
 */
void pc_init(IPluginScript *script)
{
#ifndef HW_LINUX
	Log::value("HW", "PC Windows");
	WSADATA wsaData = { 0 };
	int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (res != 0) {
		Log::err("WSAStartup failed", res);
	}
#else
	Log::value("HW", "PC Linux");
#endif
}
