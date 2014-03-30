/*
Title: Wireless Proximity Analyzer
Authros: Joe Sirianni, Ross Swerling, Cal Pittner
*/

#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <iostream>
#include <netinet/if_ether.h>

int main() {
	char errbuff[PCAP_ERRBUF_SIZE];
	const u_char *packet;
	struct pcap_pkthdr header;
	struct ether_header *eptr;
	u_char *ptr;

	pcap_t *handler = pcap_create("wlan0",errbuff);
	if (handler == NULL) //Ensures capture handle was created
	{
		std::cout << "pcap_create failed: " << errbuff << "\n";
		return EXIT_FAILURE;
	}
	
	if(pcap_set_rfmon(handler,1)==0) //Sets and checks monitor mode
	{
		std::cout << "monitor mode enabled" << "\n";
	}

	//Set capture settings
	pcap_set_snaplen(handler, 1500); //Packet buffer size
	pcap_set_promisc(handler, 0); //Promiscuous mode 0=off
	pcap_set_timeout(handler, 512); //Time waiting for packets in ms

	if(pcap_activate(handler) == 0) { //Check for successful handle activation
		std::cout << "Capturing...\n";
		std::cout << "Link-Layer Header: " << pcap_datalink(handler) << "\n";
	}
	else {
		std::cout << "Failed to begin capture\n";
		exit(1);
	}

	while(1)
	{
		packet = pcap_next(handler, &header); //Retrieves the next packet
		eptr = (struct ether_header *) packet;
		//std::cout << header.ts.tv_sec << " - ";
		ptr = (*eptr).ether_shost;

		int i = 16;
		do { //Filtering out RadioTap Header for now
                        printf("",(i == 16),*ptr++);
                }while(--i>0);
		
		i = ETHER_ADDR_LEN;
		do { //Prints first 6 bytes of 802.11 Header (source mac)
			printf("%s%x",(i == ETHER_ADDR_LEN) ? " " : ":",*ptr++);
		}while(--i>0);
		std::cout << "\n";
	}

	pcap_close(handler);
	return EXIT_SUCCESS;
}
