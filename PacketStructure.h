#ifndef PACKETSTRUCTURE_H
#define PACKETSTRUCTURE_H

#include <time.h>

struct packet_structure {
	time_t 		epoch_time;
	uint8_t 	addr[6];
	int 		dbm;
} __attribute__((__packed__));

#endif //PACKETSTRUCTURE_H