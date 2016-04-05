#ifndef __SERIAL_H
#define __SERIAL_H

class FileBase
{
public:
    void open(const char* fname);

    /** Do ioctl command for the device.
     */
    void ioc(int command, void *data);
    void write(char buffer[], int size);
    void read(char buffer[], int size);

    void close();
    
protected:
    int _handle;
};


//------------------------------------------------------------------------------
class RS232 : public FileBase
{
public:
    RS232(const char* filename);

};

//------------------------------------------------------------------------------
class SPI : public FileBase
{
public:
    SPI(const char* filename, int mode, int lsb, int bits, int speed);

    void readWrite(unsigned char *buffer, int size);
};


//------------------------------------------------------------------------------
class I2C : public FileBase
{
public:
    I2C(const char* filename, int addr);
};

#endif // __SERIAL_H
