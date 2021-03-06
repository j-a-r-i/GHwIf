#pragma once
// pin config for drivers
//


typedef enum pin {
	PIN_LED1 = 0,
	PIN_LED2,
	PIN_DS1820a,
	PIN_DS1820b,
	PIN_DS1820c,
	PIN_RFM12_SEL1,
	PIN_RFM12_IRQ1,
	PIN_RFM12_SEL2,
	PIN_RFM12_IRQ2,

	PIN_UART1_TX,
	PIN_UART1_RX,
	PIN_UART2_TX,
	PIN_UART2_RX,

	PIN_SPI1_SCK,
	PIN_SPI1_MISO,
	PIN_SPI1_MOSI,
	PIN_SPI2_SCK,
	PIN_SPI2_MISO,
	PIN_SPI2_MOSI,

	PIN_MAX
} pin_t;