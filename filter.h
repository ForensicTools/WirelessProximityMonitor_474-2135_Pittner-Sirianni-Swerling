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

class FILTER
{
public:
	FILTER();
	virtual ~FILTER();

	bool filterPacket(packet_structure, std::list<packet_structure>);
	bool setFilter(std::string);
	void clearFilter(void);

protected:
	struct compareMac {
		bool operator()(packet_structure const * a,
						packet_structure const * b) {
			return (a->mac).compare(b->mac);
		}
	};
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
		bool uniqMacFirst;
		bool uniqMacLast;

	bool blacklist;
	bool whitelist;

private:
};

#endif //FILTER_H