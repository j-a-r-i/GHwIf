#include <gtest/gtest.h>
#include <chrono>
#include <thread>
extern "C" {
  #include "hal.h"
}


void delay_sec()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

TEST(Rpi, Io1)
{
    io_init();

    for (int i=0; i<1000; i++) {
	io_set(PIN_NRF24_CE);
	delay_sec();
	
	io_clear(PIN_NRF24_CE);
	delay_sec();
    }
}


TEST(Rpi, Io2)
{
    io_init();

    for (int i=0; i<1000; i++) {
	io_set(PIN_NRF24_CSN);
	delay_sec();

	io_clear(PIN_NRF24_CSN);
	delay_sec();	
    }
}
