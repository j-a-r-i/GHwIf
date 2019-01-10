#ifndef __SERIAL_H
#define __SERIAL_H

#include <algorithm>
#include <vector>
#include "measures.h"

constexpr int HANDLE_ERROR = -1;

//------------------------------------------------------------------------------
class FileBase
{
public:
    FileBase(const char* n) :
    name(n)
    {
    }
    
    virtual ~FileBase() {
    }
    
    void open(const char* fname);

    /** Do ioctl command for the device.
     */
    int ioc(int command, void *data);
    void write(char buffer[], int size);

    int  read(void *buffer, int size);
    int  read(char buffer[], int size);
    int  read(uint64_t *val);

    void close();

    void dump();
    
    int getHandle() const {
      return _handle;
    }

    const char *getName() {
	return name.c_str();
    }
    
    virtual void HandleSelect() {
    }
    
//protected:
public:
    int _handle;
protected:
    std::string name;
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
					return (b->getHandle() == item->getHandle());
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
    RS232(const char* filename, Measure *m);

    void HandleSelect() override;

private:
    Measure *measure;
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
