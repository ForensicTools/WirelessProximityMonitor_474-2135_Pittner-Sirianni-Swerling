/*
Compile: g++ btpa.cpp -lpcap -o <file>
Dependency: libpcap-1.5.3 (need to install libbluetooth-dev)

Title: Wireless Proximity Analyzer
Authors: Joe Sirianni, Ross Swerling, Cal Pittner
*/

// header files
#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <iostream>
#include <netinet/if_ether.h>
#include <libpcap-1.5.3/pcap-bt-linux.h>
#include <libpcap-1.5.3/pcap/pcap.h>
#include <bluetooth/bluetooth.h>
#include <pcap/pcap.h>

int main()
{
	// variables
	char ebuf[PCAP_ERRBUF_SIZE];
	char err_str[PCAP_ERRBUF_SIZE];
	const u_char *bt_packet;
	struct pcap_pkthdr header;
	int * is_ours;
	int devices;
	
	// searches for available devices
	devices = bt_findalldevs(&alldevsp,err_str);

	// reads in device, error buffer, and a pointer to an integer and creates a capture handle
	pcap_t *bt_handle = bt_create("hci0",ebuf,is_ours);
	if (bt_handle == NULL)
	{
		std::cout << "bt_create failed: " << ebuf << "\n";
		return EXIT_FAILURE;
	}	
	
	// sets monitor mode
	if (pcap_set_rfmon(bt_handle,1) == 0)
	{
		std::cout << "monitor mode enabled" << "\n";
	}
	
	// capture settings
	pcap_set_snaplen(bt_handle, 1000);	// buffer size
	pcap_set_timeout(bt_handle, 512);	// timeout count in ms

}
