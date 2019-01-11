#pragma once
/** Old select based event loop. It's better to use libuv based EventLoop.
 */

#include "file.h"
#include <vector>
#include <algorithm>

#ifdef HW_LINUX
#  include <netinet/in.h>
#else
#  pragma comment(lib,"Ws2_32.lib")
#  include <WinSock2.h>
#endif

 //------------------------------------------------------------------------------
/** fd based timer
 */
class FileTimer : public FileBase
{
public:
	FileTimer(unsigned int sec);

	void HandleSelect();
};

class EventLoopSelect;

//------------------------------------------------------------------------------
class SocketServer : public FileBase
{
public:
	SocketServer(int port, EventLoopSelect *loop);

	void HandleSelect() override;

	sockaddr_in address;

private:
	EventLoopSelect* _loop;
};

//------------------------------------------------------------------------------
class Socket : public FileBase
{
public:
	Socket(int hndl);

	void HandleSelect() override;
};

//------------------------------------------------------------------------------
class EventLoopSelect
{
public:
	EventLoopSelect();
	~EventLoopSelect();

	void add(FileBase* item) {
		_items.push_back(item);
	}

	void del(FileBase* item) {
		_items.erase(std::remove_if(_items.begin(),
			_items.end(),
			[&item](FileBase *b) {
			return (b->getHandle() == item->getHandle());
		}
		),
			_items.end());
	}

	void run();

private:
	std::vector<FileBase*> _items;
};
