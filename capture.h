/*/
 * Project: Wireless Proximity Analyzer
 *
 * Repository: https://github.com/ForensicTools/WirelessProximityMonitor_474-2135_Pittner-Sirianni-Swerling
 *
 * Authors:
 *		Joe Sirianni
 *		Cal Pittner
 *		Ross Swerling
 * 
 * License: Apache v2
/*/


#ifndef WPA_CAPTURE_H
#define WPA_CAPTURE_H

#include <stdlib.h>
#include <pcap.h>
#include <unistd.h>
#include <bitset>
#include <stdint.h>
#include <pthread.h>
#include <stdio.h>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include "rtheader.h"
#include "wheader.h"
#include "PacketStructure.h"

class Capture {
public:
	Capture(std::string);
	virtual ~Capture();
	packet_structure CapturePacket(void);
	std::string convertArray(uint8_t[]);
	std::string captureError(void);

	static const int RADIOTAP_HEADER_SIZE = 8;

protected:
	std::string errorStr;
	const u_char *packet; 			//Holds pcap_next packet data
	struct pcap_pkthdr header; 		//Capture handle
	struct packet_structure packet_data;
	char errbuff[PCAP_ERRBUF_SIZE];	//Buffer for error
	pcap_t *handler;
	int db_offset_size;

private:
	
};
#endif //WPA_CAPTURE_H