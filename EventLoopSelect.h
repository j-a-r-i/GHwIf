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
class FileList
{
public:
	FileList() {
	}

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

public:
	std::vector<FileBase*> _items;
};

/** fd based timer
 */
class FileTimer : public FileBase
{
public:
	FileTimer(unsigned int sec);

	void HandleSelect();
};

//------------------------------------------------------------------------------
class SocketServer : public FileBase
{
public:
	SocketServer(int port, FileList *handles);

	void HandleSelect() override;

	sockaddr_in address;

private:
	FileList* _handles;
};

//------------------------------------------------------------------------------
class Socket : public FileBase
{
public:
	Socket(int hndl, FileList *handles);

	void HandleSelect() override;

private:
	FileList* _handles;
};

//------------------------------------------------------------------------------
class EventLoopSelect
{
public:
	EventLoopSelect();
	~EventLoopSelect();
};

