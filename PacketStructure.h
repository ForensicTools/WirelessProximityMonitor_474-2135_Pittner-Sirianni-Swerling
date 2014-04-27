#ifndef PACKETSTRUCTURE_H
#define PACKETSTRUCTURE_H

#include <time.h>
#include <iostream>
#include <string>

struct packet_structure {
	time_t 		epoch_time;
	std::string	mac;
	int 		dbm;
};

#endif //PACKETSTRUCTURE_H