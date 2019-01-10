#pragma once

#include <algorithm>
#include <vector>
#include "infoitem.h"

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
class RS232 : public FileBase, public InfoReader
{
public:
    RS232(const char* filename);

    void HandleSelect() override;

	// for InfoReader
	void read() {
	}
	void print() {
	}

private:
    InfoItemReal temp1;
    InfoItemReal temp2;
    InfoItemReal temp3;
};
