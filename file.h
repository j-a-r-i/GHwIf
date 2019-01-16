#pragma once

#include "infoitem.h"

constexpr int HANDLE_ERROR = -1;

//------------------------------------------------------------------------------
class FileBase
{
public:
	FileBase(const char* n) :
		name(n),
		deleted{false}
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

	bool isDeleted() {
		return deleted;
	}

	virtual void HandleSelect() {
	}

	//protected:
public:
	int _handle;
protected:
	std::string name;
	bool deleted;
};


/** standard input reader
 */
class FileStdin : public FileBase
{
public:
    FileStdin();

    void HandleSelect();
};

/** notify if a file has changed
 */
class FileNotify : public FileBase
{
public:
    FileNotify(const char* filename);
    ~FileNotify();

    void HandleSelect();
};

/** capture unix signals
 */
class FileSignal : public FileBase
{
public:
    FileSignal();
    ~FileSignal();

    void HandleSelect();
};

//------------------------------------------------------------------------------
class RS232 : public FileBase
{
public:
	RS232(IPluginScript &scm, const char* filename);

	void HandleSelect() override;

private:
	InfoItemReal temp1;
	InfoItemReal temp2;
	InfoItemReal temp3;
};
