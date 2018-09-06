/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#include "disk.h"
#include <iostream>
#include <sys/statvfs.h>

DiskItem::DiskItem(const char* mntPoint) :
    mountPoint(mntPoint)
{
    size = 0;
}

void DiskItem::read()
{
    struct statvfs vfs;
    int status;

    status = statvfs(mountPoint, &vfs);
    if (status == 0) {
	size = vfs.f_bsize * vfs.f_bavail / (1024L * 1024L);
    }
    else {
	std::cout << "ERROR reading " << mountPoint << " errno=" << errno << std::endl;
    }
}

void DiskItem::print()
{
    std::cout << mountPoint << " : ";
	
    if (size < 5L * 1024L)
	std::cout << size << "M";
    else
	std::cout << size / 1024L << "G";

    std::cout << std::endl;
}

//------------------------------------------------------------------------------
Disk::Disk()
{
    items.push_back(new DiskItem("/"));
    items.push_back(new DiskItem("/mnt"));
}

void Disk::print()
{
    for (auto item : items) {
	item->print();
    }
}

void Disk::read()
{
    for (auto item : items) {
	item->read();
    }
}

DiskItem *Disk::get(uint index)
{
    if (index < items.size())
	return items[index];

    return NULL;
}
