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


#ifndef FILTER_H
#define FILTER_H

#include <stack>
#include <time.h>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>
#include <list>
#include <unistd.h>
#include "PacketStructure.h"
#include "ReadWriteFiles.h"

class FILTER
{
public:
	FILTER();
	virtual ~FILTER();

	//Returns if the packets should be filtered
	bool filterPacket(packet_structure, std::list<packet_structure>);

	//Parses user input for filter settings
	//Sets variables
	bool setFilter(std::string);

	//Clears the filter that was set
	void clearFilter(void);

protected:
	//Filtering variables
	bool filterTime;
		std::string startTime;
		std::string endTime;

	bool filterDate;
		std::string startDate;
		std::string endDate;

	bool filterDay;
		std::vector<std::string> matchDays;

	bool filterMonth;
		std::vector<std::string> matchMonths;

	bool filterDbm;
		int maxDbm;
		int minDbm;

	bool uniqMac;

	bool macMono;
		std::string macAddr;

	bool blackList;
	bool whiteList;
		std::list<packet_structure> macList;

private:
};

#endif //FILTER_H