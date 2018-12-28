/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#include "disk.h"
#include <iostream>
#include <sys/statvfs.h>

DiskItem::DiskItem(const char* mntPoint) :
    InfoItem(mntPoint),
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
Disk::Disk() : InfoReader("disk")
{
    items.push_back(new DiskItem("/"));
    items.push_back(new DiskItem("/mnt"));

    for (auto& disk : items) {
	infos.push_back(disk);
    }
}

Disk::~Disk()
{
}

void Disk::print()
{
    for (auto& item : items) {
	item->print();
    }
}

void Disk::read()
{
    for (auto& item : items) {
	item->read();
    }
}
