#include "common.h"
#include "serial.h"
#include <string.h>
#ifdef HW_LINUX
  #include <fcntl.h>
  #include <unistd.h>
  #include <sys/ioctl.h>
  #include <linux/spi/spidev.h>
  #include <linux/i2c.h>
  #include <linux/i2c-dev.h>
  #include <termios.h>
#else
  #define O_NOCTTY 0
  #define O_NDELAY 0
#endif
#include <iostream>
#include <string>
#include <sstream>

void FileBase::open(const char* fname)
{
#ifdef HW_LINUX
    _handle = ::open(fname, O_RDWR| O_NOCTTY | O_NDELAY);
    if (_handle < 0) {
	Log::err("File open", fname);
	_handle = HANDLE_ERROR;
    }
#endif
}

int FileBase::ioc(int command, void *data)
{
    int stat=0;
#ifdef HW_LINUX
    if (_handle < 0)
        return 0;

    stat = ioctl(_handle, command, data);
    if (stat < 0) {
	Log::err("ioctl", command);
	return 0;
    }
#endif
    return stat;
}

void FileBase::write(char buffer[], int size)
{
    if (_handle > 0) {
#ifdef HW_LINUX
		int stat;
	
        stat = ::write(_handle, buffer, size);
        if (stat != size) {
            printf("Error writing (ret=%d)\n", stat);
        }
#endif
    }
}

int FileBase::read(void *buffer, int size)
{
    int stat = 0;
    
    if (_handle >= 0) {
#ifdef HW_LINUX
	stat = ::read(_handle, buffer, size);
        if (stat < 0) {
            printf("Error reading (ret=%d)\n", stat);
        }
#endif
    }
    return stat;
}

int FileBase::read(char buffer[], int size)
{
    int stat = read((void*)buffer, size);

    if (stat > 0)
	buffer[stat] = 0;

    return stat;
}

int FileBase::read(uint64_t *val)
{
    return read((void*)val, sizeof(uint64_t));
}


void FileBase::close()
{
#ifdef HW_LINUX
    if (_handle > 0) {
        ::close(_handle);
        _handle = -1;
    }
#endif
}

void FileBase::dump()
{
    std::cout << "\tH:" << name << " - " << _handle << std::endl;
}

//------------------------------------------------------------------------------
FileList::FileList()
{
}

//------------------------------------------------------------------------------
RS232::RS232(const char* filename, Measure *m) :
    FileBase("rs232"),
    measure(m)
{
#ifdef HW_LINUX
	struct termios  cfg;

    open(filename);
    
    if (tcgetattr(_handle, &cfg) < 0) {
	Log::err("rs232::tcgetattr", errno);
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

    if (tcsetattr(_handle, TCSAFLUSH, &cfg) < 0) {
	Log::err("rs232::tcsetattr", errno);
    }
#endif
}

void RS232::HandleSelect()
{
    const int SIZE = 80;
    char buffer[SIZE];
    int count;
    //std::cout << "HandleSelect3" << std::endl;

    count = read(buffer, SIZE);
    if (count == 0) {
		//std::cout << "connection closed " << _handle << std::endl;
    }
    else {
		if (count > 2) {
			std::string line = buffer;
			std::istringstream sstr(line);

            #define MAX_TEMP 3
            
			char command;
			int  counter;

			sstr >> command >> std::hex >> counter;

            for (int i=0; i<MAX_TEMP; i++) {
                int   temp;

                sstr >> temp;
                
                float t = temp / 16.0f;

                measure->set(i, t);
            }
	    
			std::cout << measure->getJson() << std::endl;
		}
    }
}

//------------------------------------------------------------------------------
SPI::SPI(const char* filename, int mode, int lsb, int bits, int speed) :
    FileBase("spi")
{
#ifdef HW_LINUX
	__u8 modeRead, bitsRead;
    //__u8 lsbRead;
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
#endif
}


void SPI::readWrite(unsigned char *buffer, int size)
{
#ifdef HW_LINUX
	spi_ioc_transfer xfer;

    memset(&xfer, 0, sizeof(spi_ioc_transfer));
    
    xfer.tx_buf = (__u64)buffer;
    xfer.rx_buf = (__u64)buffer;
    xfer.len    = size;
    xfer.cs_change = 1;
    xfer.delay_usecs = 0;
    
    ioc(SPI_IOC_MESSAGE(1), &xfer);
#endif
}


//------------------------------------------------------------------------------
I2C::I2C(const char* filename, int addr) :
    FileBase("i2c")
{
    open(filename);

    if (_handle > 0) {
#ifdef HW_LINUX
        int stat = ioctl(_handle, I2C_SLAVE, addr);
        if (stat < 0) {
            printf("error in ioctl I2C_SLAVE");
        }
#endif
    }
}

