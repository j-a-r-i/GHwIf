#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <sys/ioctl.h>
#include "btooth.h"
#include <iostream>

hci_filter nf;

Bluetooth::Bluetooth()
{
    _devId = hci_get_route(NULL);
    if (_devId < 0) {
        std::cout << "ERROR hci_get_route" << std::endl;
        return;
    }

    _sock = hci_open_dev(_devId);
    if (_sock < 0) {
        std::cout << "ERROR hci_open_dev" << std::endl;
        return;
    }

    hci_filter_clear(&nf);
    hci_filter_set_ptype(HCI_EVENT_PKT, &nf);
    hci_filter_set_event(EVT_LE_META_EVENT, &nf);

    if (setsockopt(_sock, SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0) {
        std::cout << "ERROR set socket HCI_FILTER" << std::endl;
		return;
    }
    
    // set device handle to non blocking mode.
    //
    /*    int on = 1;
    int stat = ioctl(_devId, FIONBIO, (char*)&on);
    if (stat < 0) {
        std::cout << "ERROR ioctl FIONBIO" << std::endl;
        }*/
}


void Bluetooth::scanStart()
{
    int stat;
    const int TIMEOUT = 1000;
    uint8_t scan_type = 0x01;
    uint16_t interval  = htobs(0x0010);
    uint16_t window    = htobs(0x0010);
    uint8_t  own_type  = 0x00;
    uint8_t  filter_policy = 0x00;
        

    stat = hci_le_set_scan_parameters(_sock, scan_type, interval, window, own_type, filter_policy, TIMEOUT);
    if (stat < 0) {
        std::cout << "ERROR hci_le_set_scan_parameters" << std::endl;
        return;
    }

    stat = hci_le_set_scan_enable(_sock, 1, 1, TIMEOUT);
    if (stat < 0) {
	std::cout << "ERROR hci_le_set_scan_enable" << std::endl;
        return;
    }  
}

void Bluetooth::scanLoop()
{
    int len;
    unsigned char buffer[HCI_MAX_EVENT_SIZE];
    char addr[18];
    evt_le_meta_event *meta;
    le_advertising_info *info;
    
    while (1) {
        len = read(_sock, buffer, sizeof(buffer));
	if (len == 0)
	    break;

	meta = (evt_le_meta_event*)(buffer + (1 + HCI_EVENT_HDR_SIZE));
	info = (le_advertising_info*)(meta->data + 1);
	
	ba2str(&info->bdaddr, addr);
	
	std::cout << "-" << meta->subevent << " - " << addr << std::endl;
        
    }
}

void Bluetooth::scanStop()
{
    int stat;
    const int TIMEOUT = 1000;

    stat = hci_le_set_scan_enable(_sock, 0, 1, TIMEOUT);
    if (stat < 0) {
        std::cout << "ERROR hci_le_set_scan_enable" << std::endl;
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
        std::cout << "ERROR hci_inquiry" << std::endl;
        free(info);
        return;
    }

    for (int i=0; i<count; i++) {
        char name[20] = { 0 };
        int stat = hci_read_remote_name(_sock, &(info[i].bdaddr), sizeof(name), name, 0);
        if (stat < 0) {
            strcpy(name, "???");
        }
        std::cout << "found: "  << name << std::endl;
    }    
    free(info);
}


void Bluetooth::close()
{
    if (_sock > 0) {
        hci_close_dev(_sock);
    }
}
