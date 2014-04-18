#ifndef WPA_CAPTURE_H
#define WPA_CAPTURE_H

#include <pcap.h>
#include <unistd.h>
#include <bitset>
#include <stdint.h>
#include "rtheader.h"
#include "wheader.h"

class CaptureText
{
public:
  Capture();
  virtual ~Capture();

protected:
	char errbuff[PCAP_ERRBUF_SIZE];	//Buffer for error
	const u_char *packet; 			//Holds pcap_next packet data
	struct pcap_pkthdr header; 		//Capture handle
	const int RADIOTAP_HEADER_SIZE = 8;
	static const int present_field_align[5] = {8,1,1,4,2}}; //RadioTap field alignment in bytes

	void CaptureThread();

	struct ieee80211_radiotap_header {
		uint8_t		it_version;		//Always 0
		uint8_t		it_pad;			//Unused padding
		uint16_t	it_len;			//Entire length of RadioTap header including data
		uint32_t	it_present;		//Fields used
	} __attribute__((__packed__));

	struct radiotap_data_flags {
		unsigned int 	pad:6;		//Unessisary flags
		unsigned int 	bad_fcs:1;	//Bad FCS flag
		unsigned int 	pad2:1;		//Another uneeded flag
	} __attribute__((__packed__));

	struct packet_data {
		int time;
		uint8_t mac[6];
		int dbm;
	};
};

#endif //WPA_CAPTURE_H