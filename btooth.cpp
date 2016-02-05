#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <sys/ioctl.h>
#include "btooth.h"

Bluetooth::Bluetooth()
{
    _devId = hci_get_route(NULL);
    if (_devId < 0) {
	printf("ERROR hci_get_route\n");
	return;
    }

    _sock = hci_open_dev(_devId);
    if (_sock < 0) {
	printf("ERROR hci_open_dev\n");
	return;
    }

    // set device handle to non blocking mode.
    //
    int on = 1;
    int stat = ioctl(_devId, FIONBIO, (char*)&on);
    if (stat < 0) {
	printf("ERROR ioctl FIONBIO");
    }
}


void Bluetooth::scanStart()
{
    int stat;
    const int TIMEOUT = 1000;

    stat = hci_le_set_scan_parameters(_sock, 0x01, htobs(0x0010), htobs(0x0010), 0x00, 0x00, TIMEOUT);
    if (stat < 0) {
	printf("ERROR hci_le_set_scan_parameters\n");
	return;
    }

    stat = hci_le_set_scan_enable(_sock, 1, 1, TIMEOUT);
    if (stat < 0) {
	printf("ERROR hci_le_set_scan_enable\n");
	return;
    }  
}

void Bluetooth::scanStop()
{
    int stat;
    const int TIMEOUT = 1000;

    stat = hci_le_set_scan_enable(_sock, 0, 1, TIMEOUT);
    if (stat < 0) {
	printf("ERROR hci_le_set_scan_enable\n");
	return;
    }
}

void Bluetooth::scan()
{
    int len  = 4;
    int maxRsp = 128;

    inquiry_info *info = (inquiry_info*)malloc(maxRsp * sizeof(inquiry_info));

    int count = hci_inquiry(_devId, len, maxRsp, NULL, &info, IREQ_CACHE_FLUSH);
    if (count < 0) {
	printf("ERROR hci_inquiry\n");
	free(info);
	return;
    }

    for (int i=0; i<count; i++) {
	char name[20] = { 0 };
	int stat = hci_read_remote_name(_sock, &(info[i].bdaddr), sizeof(name), name, 0);
	if (stat < 0) {
	    strcpy(name, "???");
	}
	printf("found: %s\n", name);
    }
    
    free(info);
}


void Bluetooth::close()
{
    if (_sock > 0)
	hci_close_dev(_sock);
}
