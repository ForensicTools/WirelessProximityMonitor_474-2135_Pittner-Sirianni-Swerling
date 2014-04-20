#include "filter.h"

static const std::string days[7] = 
	{"Mon", "Tue", "Wed", "Thu", "Fri"};
static const std::string weekEnds[2] = {"Sat", "Sun"};
static const std::string weekDays[5] = {"Mon", "Tue", "Wed", "Thu", "Fri"};

static const std::string months[12] = {""};

static const std::string filterCommands[6] = {"time", "date", "days", "months", "dbm", "uniqMac"};

FILTER::FILTER() {
	clearFilter();
}

bool FILTER::filterPacket(packet_structure packet) {
	bool answer = false;

	if(filterTime == true) {

	}

	if(filterDate == true) {

	}

	if(filterDay == true) {

	}

	if(filterMonth == true) {

	}

	if(uniqMac == true) {

	}

	return answer;
}

bool FILTER::setFilter(std::string filter) {
	clearFilter();

	bool syntaxCheck = true;
	std::stringstream ss(filter);
	std::string buff;
	std::string args;
	std::vector<std::string> commands; 

	while(ss >> buff) {
		commands.push_back(buff);
	}

	for(int i = 0; i <= commands.size(); i++) {
		buff.str(std::string());
		args.str(std::string());

		for(size_t x = 0; x <= (commands[i].size() - 1; x++) {
			while(commands[i].at(x) != "=") {
				buff += commands[i].at(x);
			}
			args += commands[i].at(x);

		for(int y = 0; y <=6; y++) {
			switch(buff) {
				case "time":
					for(size_t z = 0; z <= (args.size() - 1); z++) {
						startTime += args.at(z);
					}
					break;

				case "date":
					for(size_t z = 0; z <= (args.size() - 1); z++) {
						
					}
					break;

				case "days":
					for(size_t z = 0; z <= (args.size() - 1); z++) {
						
					}
					break;

				case "months":
					for(size_t z = 0; z <= (args.size() - 1); z++) {
						
					}
					break;

				case "dbm":
					for(size_t z = 0; z <= (args.size() - 1); z++) {
						
					}
					break;

				case "uniqMac":
					uniqueMac = true;
					break;

				default:
					syntaxCheck = false;
					break;
			}
		}
	}

	return syntaxCheck;
}

void FILTER::clearFilter() {
	filterTime = false;
		startTime.str(std::string());
		endTime.str(std::string());

	filterDate = false;
		startDate.str(std::string());
		endDate.str(std::string());

	filterDay = false;
		matchDays.erase();

	filterMonth = false;
		matchMonths.erase();

	bool filterDbm = false;
		int maxDbm = 0;
		int minDbm = 0;

	bool uniqMac = false;
}