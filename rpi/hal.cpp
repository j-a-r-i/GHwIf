#include "hw.h"
#include "hal.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <string.h> // for memset
#include "../logger.h"

#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
#define BLOCK_SIZE               (4*1024)

void *gpio_map;
volatile unsigned *gpio;

int fd_spi;  ///@todo: use port parameter to access file descriptors.
int fd_i2c;

//------------------------------------------------------------------------------
//<sample_copy>
#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0

#define GET_GPIO(g) (*(gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH

#define GPIO_PULL *(gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock
//</sample_copy>

//------------------------------------------------------------------------------
typedef enum PinMode {
    PIN_OUTPUT,
    PIN_INPUT,
} pinmode_t;

typedef struct IoPinTable {
    int pin;
    pinmode_t mode;
} IoPinTable_t;

IoPinTable_t PINMAP[] = {
    [PIN_NRF24_CE]  = { 17, PIN_OUTPUT },
    [PIN_NRF24_CSN] = { 27, PIN_OUTPUT }
};
    

//------------------------------------------------------------------------------
void delay_us(uint16_t time)
{
}

//------------------------------------------------------------------------------
/** This implementation uses memory registers.
 *
 * @see https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
 */
void io_init()
{
    int fd;
    
    if ((fd = open("/dev/gpiomem", O_RDWR|O_SYNC)) < 0) {
	Log::err(__FUNCTION__, "opening /dev/gpiomem");
	return;
    }

    gpio_map = mmap(NULL,             //Any adddress in our space will do
		    BLOCK_SIZE,       //length
		    PROT_READ | PROT_WRITE,
		    MAP_SHARED,
		    fd,
		    GPIO_BASE);       //Offset to GPIO peripheral
    
    close(fd);

    if (gpio_map == MAP_FAILED) {
	Log::err(__FUNCTION__, "mmap failed");
	return;
    }

    gpio = (volatile unsigned *)gpio_map;
}

//------------------------------------------------------------------------------
void pinMode(pin_t pin, uint8_t mode)
{
    INP_GPIO(PINMAP[pin].pin);
    if (mode == IO_MODE_OUTPUT)
	OUT_GPIO(PINMAP[pin].pin);
}

//------------------------------------------------------------------------------
void digitalWrite(pin_t pin, uint8_t value)
{
    if (value)
	GPIO_SET = 1 << (PINMAP[pin].pin);
    else
	GPIO_CLR = 1 << (PINMAP[pin].pin);
}

//------------------------------------------------------------------------------
uint8_t digitalRead(pin_t pin)
{
    if (GET_GPIO(PINMAP[pin].pin))
	return 1;
    return 0;
}

//------------------------------------------------------------------------------
void spi_init(uint8_t port)
{
    //uint8_t mode = SPI_LSB_FIRST | SPI_CPOL | SPI_3WIRE;
    uint8_t mode = SPI_MODE_3;
    uint8_t bits = 8;
    uint32_t speed = 500000;
    int ret;
	
    fd_spi = open("/dev/spidev0.0", O_RDWR);
    if (fd_spi <= 0) {
	Log::err(__FUNCTION__, "open device");
	return;
    }

    ret = ioctl(fd_spi, SPI_IOC_WR_MODE, &mode);
    if (ret == -1) {
	Log::err(__FUNCTION__, "mode setting");
	return;
    }

    ret = ioctl(fd_spi, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1) {
	Log::err(__FUNCTION__, "bits setting");
	return;
    }

    ret = ioctl(fd_spi, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1) {
	Log::err(__FUNCTION__, "set max speed");
	return;
    }

    // SPI_IOC_WR_LSB_FIRST
}

//------------------------------------------------------------------------------
uint16_t spi_write(uint8_t port, uint16_t data)
{
    int ret;
    uint16_t rval = 0;

    spi_ioc_transfer trans;

    memset(&trans, 0, sizeof(spi_ioc_transfer));
    
    trans.tx_buf = (unsigned long)&data;
    trans.rx_buf = (unsigned long)&rval;
    trans.len = 2;
    trans.delay_usecs = 0;
    trans.speed_hz = 500000;
    trans.bits_per_word = 8;

    ret = ioctl(fd_spi, SPI_IOC_MESSAGE(1), &trans);
    if (ret == -1) {
	Log::err(__FUNCTION__, "transfer");
	return 0;
    }
 
    return rval;
}


//------------------------------------------------------------------------------
void i2c_init(uint8_t port)
{
    uint8_t addr = 10;
    int ret;
	
    fd_i2c = open("/dev/i2c", O_RDWR);
    if (fd_i2c <= 0) {
	Log::err(__FUNCTION__, "open device");
	return;
    }

    ret = ioctl(fd_i2c, I2C_SLAVE, &addr);
    if (ret == -1) {
	Log::err(__FUNCTION__, "set address");
	return;
    }
}
