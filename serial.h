#ifndef __SERIAL_H
#define __SERIAL_H

#include <algorithm>
#include <vector>

//------------------------------------------------------------------------------
class FileBase
{
public:
    void open(const char* fname);

    /** Do ioctl command for the device.
     */
    void ioc(int command, void *data);
    void write(char buffer[], int size);
    int  read(char buffer[], int size);

    void close();

    int GetHandle() {
      return _handle;
    }
    
    virtual void HandleSelect() {
    }
    
//protected:
public:
    int _handle;
};

//------------------------------------------------------------------------------
class FileList
{
public:
    FileList();

    void add(FileBase* item) {
	_items.push_back(item);
    }

    void del(FileBase* item) {
	_items.erase(std::remove_if(_items.begin(),
				    _items.end(),
				    [&item](FileBase *b){
					return (b->GetHandle() == item->GetHandle());
				    }
			 ),
		     _items.end());
    }
    
public:
    std::vector<FileBase*> _items;
};

//------------------------------------------------------------------------------
class RS232 : public FileBase
{
public:
    RS232(const char* filename);

    void HandleSelect();
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
