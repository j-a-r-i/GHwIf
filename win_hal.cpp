// windows implementation of drivers hal layer (see drivers repository)
//
#include "win_hal.h"
#include "logger.h"
#include <iostream>

typedef struct uartport
{
	HANDLE  handle;
	bool    realSerial;
	const char* port_device;
}
uartport_t;


uartport_t uart[] = {
	{INVALID_HANDLE_VALUE, true,  "COM1"},
	{INVALID_HANDLE_VALUE, false, "\\\\.\\pipe\\mycom"}
};

void delay_us(uint16_t time)
{
	std::cout << "delay_us: " << time << std::endl;
}

void io_init()
{
	std::cout << "io_init" << std::endl;
}

void pinMode(pin_t, uint8_t mode)
{
	std::cout << "io_mode: " << mode << std::endl;
}

void digitalWrite(pin_t pin, uint8_t value)
{
    if (value)
	std::cout << "io_set: " << pin << std::endl;
    else
	std::cout << "io_clear: " << pin << std::endl;
}

uint8_t digitaRead(pin_t pin)
{
	std::cout << "io_read: " << pin << std::endl;
	return 0;
}

void timer2_init()
{
	std::cout << "timer2_init" << std::endl;
}

// Serial port
//
void WinSerial::begin(uint16_t rate)
{
	DCB params = { 0 };

	std::cout << "Serial::begin: " << name << std::endl;
	hnd = CreateFile(name.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,  // we do not want to share it
		NULL, // no security
		0,
		NULL, NULL);
	if (hnd == INVALID_HANDLE_VALUE) {
		Log::err(__FUNCTION__, "open");
		return;
	}

	if (realSerial) {
		params.DCBlength = sizeof(params);
		if (GetCommState(hnd, &params) == 0) {
			Log::err(__FUNCTION__, "GetCommState");
			return;
		}
		params.BaudRate = rate;
		params.ByteSize = 8;
		params.Parity = NOPARITY;
		params.StopBits = ONESTOPBIT;

		if (SetCommState(hnd, &params) == 0) {
			Log::err(__FUNCTION__, "SetCommState");
			return;
		}
	}
	else {
		// do something for pipe
	}
}

void WinSerial::write(uint8_t ch)
{
	DWORD bytesWritten = 0;

	if (WriteFile(hnd,
		&ch,
		1,
		&bytesWritten,
		NULL) == 0)
	{
		Log::err("uart.WriteFile", GetLastError());
		return;
	}

	if (bytesWritten != 1) {
		Log::err("uart.WriteFile incomplete", name.c_str());
		return;
	}
}

void WinSerial::end()
{
	if (CloseHandle(hnd)) {
		Log::err("uart::Close", name.c_str());
	}
}
