#pragma once

#include "hw.h"
#include "hal.h"
#include <Windows.h>
#include <string>

class WinSerial : public SerialBase
{
public:
	WinSerial(const char* n, bool rs) :
		name{ n },
		realSerial{ rs }
	{
	}

	void begin(uint16_t rate);
	void write(uint8_t ch);
	void end();

private:
	std::string name;
	bool realSerial;
	HANDLE hnd;
};

// Serial port instances
//
extern WinSerial Serial;
extern WinSerial Serial1;