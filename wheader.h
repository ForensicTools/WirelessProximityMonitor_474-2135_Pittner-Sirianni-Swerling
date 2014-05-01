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


#ifndef WHEADER_H
#define WHEADER_H

#include <stdint.h>

struct ieee80211_mac_header {
	unsigned int	protoVer:2;
	unsigned int 	type:2;
	unsigned int 	subType:4;
	unsigned int 	toDS:1;
	unsigned int 	fromDS:1;
	unsigned int 	moreFrag:1;
	unsigned int 	retry:1;
	unsigned int 	pwrMgmt:1;
	unsigned int 	moreData:1;
	unsigned int 	wep:1;
	unsigned int 	order:1;

	uint16_t		durID;
	uint8_t 	 	addr1[6];
	uint8_t		 	addr2[6];
	uint8_t			addr3[6];
	uint16_t		seqCtrl;
	uint8_t 	 	addr4[6];
} __attribute__((__packed__));

#endif //WHEADER_H