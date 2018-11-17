#pragma once

#include "serial.h"

/** fd based timer
 */
class FileTimer : public FileBase
{
public:
    FileTimer(unsigned int sec);
    
    void HandleSelect();
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

