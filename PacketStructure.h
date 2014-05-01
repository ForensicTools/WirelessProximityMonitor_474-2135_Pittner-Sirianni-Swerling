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