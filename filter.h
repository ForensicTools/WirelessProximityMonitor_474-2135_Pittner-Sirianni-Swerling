#ifndef FILTER_H
#define FILTER_H

#include <stack>
#include <time.h>
#include <sstream>
#include <string>
#include <vector>
#include "PacketStructure.h"

class MainWindow : public Gtk::Window
{
public:
	FILTER();
	virtual ~FILTER();

	bool filterPacket(packet_structure);
	bool setFilter(std::string);
	void clearFilter(void);

protected:
	bool filterTime;
		std::string startTime;
		std::string endTime;

	bool filterDate;
		std::string startDate;
		std::strnig endDate;

	bool filterDay;
		vector<std::string> matchDays;

	bool filterMonth;
		vector<std::string> matchMonths;

	bool filterDbm;
		int maxDbm;
		int minDbm;

	bool uniqMac;

private:
};

#endif //FILTER_H