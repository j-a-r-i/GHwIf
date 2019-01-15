// windows implementation of drivers hal layer (see drivers repository)
//
#include "hw.h"
#include "hal.h"
#include "logger.h"
#include <iostream>
#include <Windows.h>

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

void io_pin_init(pin_t pin, uint8_t mode)
{
	std::cout << "io_pin_init: " << pin << ", " << mode << std::endl;
}

void io_mode(pin_t, uint8_t mode)
{
	std::cout << "io_mode: " << mode << std::endl;
}

void io_set(pin_t pin)
{
	std::cout << "io_set: " << pin << std::endl;
}

void io_clear(pin_t pin)
{
	std::cout << "io_clear: " << pin << std::endl;
}

uint8_t io_read(pin_t pin)
{
	std::cout << "io_read: " << pin << std::endl;
	return 0;
}

void uart_init(uint8_t port)
{
	HANDLE hnd;
	DCB params = { 0 };

	std::cout << "uart_init: " << port << std::endl;
	hnd = CreateFile(uart[port].port_device,
		GENERIC_READ | GENERIC_WRITE,
		0,  // we do not want to share it
		NULL, // no security
		0,
		NULL, NULL);
	if (hnd == INVALID_HANDLE_VALUE) {
		Log::err(__FUNCTION__, "open");
		return;
	}

	if (uart[port].realSerial) {
		params.DCBlength = sizeof(params);
		if (GetCommState(hnd, &params) == 0) {
			Log::err(__FUNCTION__, "GetCommState");
			return;
		}
		params.BaudRate = CBR_115200;
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
	uart[port].handle = hnd;
}
static void uart_internal_write(uint8_t port, buffer_t *buf)
{
	DWORD bytesWritten = 0;
	if (WriteFile(uart[port].handle,
		buf->data,
		buf->size,
		&bytesWritten,
		NULL) == 0)
	{
		Log::err("uart.WriteFile", GetLastError());
		return;
	}

	if (bytesWritten != buf->size) {
		Log::err("uart.WriteFile incomplete", (buf->size - bytesWritten));
		return;
	}
}
void uart_print(uint8_t port, buffer_t *buf)  // async send command
{
	std::cout << "uart_print: " << port << ", " << buf << std::endl;

	uart_internal_write(port, buf);
}

void uart_sync(uint8_t port, buffer_t *buf)
{
	std::cout << "uart_sync: " << port << ", " << buf << std::endl;

	uart_internal_write(port, buf);
}

void uart_send(uint8_t port, char ch)    // old version use uart_print
{
	std::cout << "uart_send: " << port << ", " << ch << std::endl;

	buffer_t buf;

	buf.data[0] = ch;
	buf.data[1] = 0;
	buf.size = 1;

	uart_internal_write(port, &buf);
}

void uart_sends(uint8_t port, char *buf)
{
	std::cout << "uart_sends: " << port << ", " << buf << std::endl;

	buffer_t b;

	strcpy((char*)b.data, buf);
	b.size = static_cast<uint8_t>(strlen(buf));

	uart_internal_write(port, &b);
}

void uart_send_nl(uint8_t port)
{
	std::cout << "uart_send_nl: " << port << std::endl;
	buffer_t buf;

	buf.data[0] = '\r';
	buf.data[1] = '\n';
	buf.data[2] = 0;
	buf.size = 2;

	uart_internal_write(port, &buf);
}

void timer2_init()
{
	std::cout << "timer2_init" << std::endl;
}

void spi_init(uint8_t port)
{
	std::cout << "spi_init: " << port << std::endl;
}

uint16_t spi_write(uint8_t port, uint16_t data)
{
	std::cout << "spi_write: " << port << ", " << data << std::endl;
	return 0;
}
