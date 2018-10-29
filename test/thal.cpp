#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include <stdio.h>
extern "C" {
  #include "hal.h"
}


void delay_sec()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

TEST(Hal, Io1)
{
    io_init();
    io_mode(PIN_NRF24_CE, IO_MODE_OUTPUT);
    
    for (int i=0; i<80; i++) {
	io_set(PIN_NRF24_CE);
	delay_sec();
	
	io_clear(PIN_NRF24_CE);
	delay_sec();

	printf(".");
    }
    printf("\n");
}


TEST(Hal, Io2)
{
    io_init();
    io_mode(PIN_NRF24_CSN, IO_MODE_OUTPUT);

    for (int i=0; i<80; i++) {
	io_set(PIN_NRF24_CSN);
	delay_sec();

	io_clear(PIN_NRF24_CSN);
	delay_sec();

	printf(".");
    }
    printf("\n");
}

TEST(Hal, Spi1)
{
    spi_init(0);

    for (int i=0; i<80; i++) {
	spi_write(0, 0xF0F0);
	delay_sec();

	printf(".");
    }
    printf("\n");
}
