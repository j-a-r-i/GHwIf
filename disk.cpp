/******************************************************************************
 * Copyright (C) 2018 Jari Ojanen
 ******************************************************************************/
#include "disk.h"
#include "logger.h"
#include <sys/statvfs.h>

//------------------------------------------------------------------------------
Disk::Disk(IPluginScript& scm) :
	diskRoot{scm, "disk-root"},
	diskMnt{ scm, "disk-mnt" }
{
}

Disk::~Disk()
{
}

double Disk::free(const char* mountPoint)
{
	struct statvfs vfs;
	long size = 0;
	int status;

	status = statvfs(mountPoint, &vfs);
	if (status == 0) {
		size = vfs.f_bsize * vfs.f_bavail / (1024L * 1024L);
	}
	else {
		Log::err("DiskRead", errno);
	}

	return size / 1024.0;  // value in gigas
}

void Disk::operator()()
{
	diskRoot.setValue(free("/"));
	diskMnt.setValue(free("/mnt"));
}
