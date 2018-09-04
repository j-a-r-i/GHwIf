/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#include "disk.h"
#include <iostream>
#include <sys/statvfs.h>

Disk::Disk()
{
}

void Disk::readDisk(const char* mountpoint)
{
    struct statvfs vfs;
    int status;

    status = statvfs(mountpoint, &vfs);
    if (status == 0) {
	long freeM = vfs.f_bsize * vfs.f_bavail / (1024L * 1024L);

	std::cout << mountpoint << " : ";
	
	if (freeM < 5L * 1024L)
	    std::cout << freeM << "M";
	else
	    std::cout << freeM / 1024L << "G";

	std::cout << std::endl;

		
    }
    else {
	std::cout << "ERROR reading " << mountpoint << " errno=" << errno << std::endl;
    }
}


void Disk::read()
{
    readDisk("/");
    readDisk("/mnt");
}
