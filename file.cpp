#include "config.h"
#ifdef HW_LINUX
#include <sys/timerfd.h>
#include <sys/signalfd.h>
#include <sys/inotify.h>
#include <unistd.h>
#endif
#include <signal.h>
#include "file.h"
#include "logger.h"

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

    ts.it_interval.tv_sec  = sec;
    ts.it_interval.tv_nsec = 0;
    ts.it_value.tv_sec  = sec;
    ts.it_value.tv_nsec = 0;

    err = timerfd_settime(_handle, 0, &ts, NULL);
    if (err <0) {
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
FileStdin::FileStdin() :
    FileBase("stdin")
{
    _handle = STDIN_FILENO;
}

void FileStdin::HandleSelect()
{
    const int BUFFER_SIZE = 80;
    char buffer[BUFFER_SIZE];
    
    read(buffer, BUFFER_SIZE);

    Log::msg("stdin", buffer);
}

//------------------------------------------------------------------------------
#ifdef HW_LINUX
FileNotify::FileNotify(const char *filename) :
    FileBase("notify")
{
    int err;
    
    _handle = inotify_init();
    if (_handle == HANDLE_ERROR) {
	Log::err("inotify_init", errno);
	return;
    }

    err = inotify_add_watch(_handle, filename, IN_ALL_EVENTS);
    if (_handle == HANDLE_ERROR) {
	Log::err("inotify_add_watch", errno);
	return;
    }
}

FileNotify::~FileNotify()
{
}

void FileNotify::HandleSelect()
{
    struct inotify_event event;

    read(&event, sizeof(event));
    
    Log::msg("notify", event.mask);
}

//------------------------------------------------------------------------------
FileSignal::FileSignal() :
    FileBase("signal")
{
    sigset_t mask;
    int err;

    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);
    
    _handle = signalfd(-1, &mask, 0);
    if (_handle == HANDLE_ERROR) {
	Log::err("signalfd", errno);
	return;
    }
}

FileSignal::~FileSignal()
{
}

void FileSignal::HandleSelect()
{
    struct signalfd_siginfo siginfo;

    read(&siginfo, sizeof(struct signalfd_siginfo));
	
    Log::msg("signal", 1);
}
#endif
