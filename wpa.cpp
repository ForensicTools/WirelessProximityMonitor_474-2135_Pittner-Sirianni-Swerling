/*
Compile: g++ wpa.cpp -lpcap -o <file>
Dependency: libpcap-1.5.3

Title: Wireless Proximity Analyzer
Authros: Joe Sirianni, Ross Swerling, Cal Pittner
*/

#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <iostream>
#include <iomanip>
#include <netinet/if_ether.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <bitset>
#include "rtheader.h"
#include "wheader.h"

void* iterate_wifi_channels(void*);

int main() {
	char errbuff[PCAP_ERRBUF_SIZE];	//Buffer for error
	const u_char *packet; 			//Holds pcap_next packet data
	struct pcap_pkthdr header; 		//Capture handle
	pthread_t chanThread;			//Thread for rotating WiFi channels

	pcap_t *handler = pcap_create("wlan0",errbuff); //Creates handle for device

	if (handler == NULL) 			//Ensures capture handle was created
	{
		std::cout << "pcap_create failed: " << errbuff << "\n";
		return EXIT_FAILURE;
	}
	
	if(pcap_set_rfmon(handler,1)==0) //Sets and checks monitor mode
	{
		std::cout << "monitor mode enabled" << "\n";
	}

	//Set capture settings
	pcap_set_snaplen(handler, 64);	//Packet buffer size
	pcap_set_promisc(handler, 0);	//Promiscuous mode 0=off
	pcap_set_timeout(handler, 512);	//Time waiting for packets in ms

	if(pcap_activate(handler) == 0) { //Check for successful handle activation
		std::cout << "Capturing...\n";
		std::cout << "Link-Layer Header: " << pcap_datalink(handler) << "\n";
	}
	else {
		std::cout << "Failed to begin capture\n";
		exit(1);
	}

	//Begin Channel rotation
	pthread_create(&chanThread, NULL, &iterate_wifi_channels, NULL);

	while(1)
	{
		packet = pcap_next(handler, &header); //Retrieves the next packet
		struct ieee80211_radiotap_header *rh_ptr = (struct ieee80211_radiotap_header *) (packet); //RadioTap header
		struct rtapdata *rtd_ptr = (struct rtapdata *) (packet + 8); //RadioTap Data
		struct ieee80211_mac_header *mh_ptr = (struct ieee80211_mac_header *) (packet + (*rh_ptr).it_len); //802.11 Header

		//Time column
		time_t epoch_time;
		struct tm * ptm;
		epoch_time = header.ts.tv_sec;	//Time of packet recieved
		ptm = gmtime (&epoch_time);		//Structures time
		std::cout << std::setbase(10) << "[" << (ptm->tm_year+1900) << "-" << (ptm->tm_mon+1) << "-" << (ptm->tm_mday)		//Date Time
			<< " " << " " << (ptm->tm_hour) << ":" << (ptm->tm_min) << ":" << (ptm->tm_sec) << "] ";	//[yyyy-mm-dd hh-mm-ss]

		//MAC column - MAC addr Changes based on flags present
		if(mh_ptr->fromDS == 0) {
			std::cout << "[";
			for(int i = 0; i<= 5; i++) {
				printf("%s%02X", (i==0)?"":":",mh_ptr->addr2[i]);
			}
			std::cout << "] ";
		}
		else if(mh_ptr->toDS == 0) {
			std::cout << "[";
			for(int i = 0; i<= 5; i++) {
				printf("%s%02X", (i==0)?"":":",mh_ptr->addr3[i]);
			}
			std::cout << "] ";
		}
		else {
			std::cout << "[";
			for(int i = 0; i<= 5; i++) {
				printf("%s%02X", (i==0)?"":":",mh_ptr->addr4[i]);
			}
			std::cout << "] ";
		}

		//Signal strength column
		std::cout << "[" << (int)rtd_ptr->db_antenna_signal << " dB]\n";
	}

	pcap_close(handler);
	pthread_exit(&chanThread);
	return EXIT_SUCCESS;
}

void* iterate_wifi_channels(void*) {
	while(1) {
		system("iwconfig wlan0 channel 1");
		usleep(1000 * 1000);
		system("iwconfig wlan0 channel 6");
		usleep(1000 * 1000);
		system("iwconfig wlan0 channel 11");
		usleep(1000 * 1000);
	}
}	
