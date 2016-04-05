#include "serial.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <termios.h>
#include <unistd.h>

void FileBase::open(const char* fname)
{
    _handle = ::open(fname, O_RDWR| O_NOCTTY | O_NDELAY);
    if (_handle < 0) {
        printf("error opening SPI bus!\n");
    }
}

void FileBase::ioc(int command, void *data)
{
    if (_handle < 0)
        return;

    int stat;

    stat = ioctl(_handle, command, data);
    if (stat < 0) {
        printf("error in ioctl %d\n", command);
    }
}

void FileBase::write(char buffer[], int size)
{
    if (_handle > 0) {
        int stat;
	
        stat = ::write(_handle, buffer, size);
        if (stat != size) {
            printf("Error writing\n");
        }
    }
}

void FileBase::read(char buffer[], int size)
{
    if (_handle > 0) {
        int stat;
	
        stat = ::read(_handle, buffer, size);
        if (stat != size) {
            printf("Error reading\n");
        }
    }
}

void FileBase::close()
{
    if (_handle > 0) {
        ::close(_handle);
        _handle = -1;
    }
}

//------------------------------------------------------------------------------
RS232::RS232(const char* filename)
{
    struct termios  cfg;

    if (tcgetattr(handle, &cfg) < 0) {
        printf("Error tcgetattr\n");
    }

    /*if (cfsetispeed(&cfg, B9600) < 0) {
        printf("Error cfsetispeed\n");
    }
    if (cfsetospeed(&cfg, B9600) < 0) {
        printf("Error cfsetospeed\n");
        }*/

    cfg.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    cfg.c_iflag = IGNPAR | ICRNL;
    cfg.c_oflag = 0;
    cfg.c_lflag = ICANON;

    if (tcsetattr(handle, TCSAFLUSH, &cfg) < 0) {
        printf("Error tcsetattr\n");
    }
}

//------------------------------------------------------------------------------
SPI::SPI(const char* filename, int mode, int lsb, int bits, int speed)
{
    __u8 modeRead, bitsRead;
    __u8 lsbRead;
    __u32 speedRead;

    open(filename);
    
    ioc(SPI_IOC_WR_MODE, &mode);

    ioc(SPI_IOC_RD_MODE, &modeRead);

    ioc(SPI_IOC_WR_BITS_PER_WORD, &bits);

    ioc(SPI_IOC_RD_BITS_PER_WORD, &bitsRead);

    //ioc(SPI_IOC_WR_LSB_FIRST, &lsb);

    //ioc(SPI_IOC_RD_LSB_FIRST, &lsbRead);  

    ioc(SPI_IOC_WR_MAX_SPEED_HZ, &speed);

    ioc(SPI_IOC_RD_MAX_SPEED_HZ, &speedRead);  
}


void SPI::readWrite(unsigned char *buffer, int size)
{
    spi_ioc_transfer xfer;

    memset(&xfer, 0, sizeof(spi_ioc_transfer));
    
    xfer.tx_buf = (__u64)buffer;
    xfer.rx_buf = (__u64)buffer;
    xfer.len    = size;
    xfer.cs_change = 1;
    xfer.delay_usecs = 0;
    
    ioc(SPI_IOC_MESSAGE(1), &xfer);
}


//------------------------------------------------------------------------------
I2C::I2C(const char* filename, int addr)
{
    open(filename);

    if (_handle > 0) {
        int stat = ioctl(_handle, I2C_SLAVE, addr);
        if (stat < 0) {
            printf("error in ioctl I2C_SLAVE");
        }
    }
}

