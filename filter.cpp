#include "filter.h"

static const std::string days[7] = 
	{"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const std::string weekEnds[2] = {"Sat", "Sun"};
static const std::string weekDays[5] = 
	{"Mon", "Tue", "Wed", "Thu", "Fri"};

static const std::string months[12] = 
	{"Jan", "Feb", "Mar", "Apr", "May","Jun",
	 "Jul","Aug","Sep", "Oct", "Nov", "Dec"};

static const std::string filterCommands[6] = 
	{"time", "date", "days", "months", "dbm", "uniqMac"};

FILTER::FILTER() {
	clearFilter();
}

FILTER::~FILTER() {

}

bool FILTER::filterPacket(packet_structure packet,
				std::list<packet_structure> packetList) {
	bool answer = false;
	time_t epoch_time = packet.epoch_time;
	struct tm *ptm = gmtime (&epoch_time);

	if(filterTime == true) {
		std::stringstream timeSs;
		timeSs << std::setfill('0');

		//Time in readable format
		timeSs << std::setw(2) << static_cast<unsigned>(ptm->tm_hour);
		timeSs << ":";
		timeSs << std::setw(2) << static_cast<unsigned>(ptm->tm_min);
		timeSs << ":";
		timeSs << std::setw(2) << static_cast<unsigned>(ptm->tm_sec);

		if(startTime.compare(endTime) < 0)
			if((timeSs.str()).compare(startTime) < 0 ||
				(timeSs.str()).compare(endTime) > 0)
				answer = true;
		else if(startTime.compare(endTime) > 0)
			if((timeSs.str()).compare(startTime) > 0 ||
				(timeSs.str()).compare(endTime) < 0)
				answer = true;
	}

	if(filterDate == true && answer == false) {
		std::stringstream dateSs;
		dateSs << std::setfill('0');

		//Date in readable format
		dateSs << std::setw(4) << static_cast<unsigned>(ptm->tm_year+1900);
		dateSs << "/";
		dateSs << std::setw(2) << static_cast<unsigned>(ptm->tm_mon+1);
		dateSs << "/";
		dateSs << std::setw(2) << static_cast<unsigned>(ptm->tm_mday);

		if(startDate.compare(endDate) < 0)
			if((dateSs.str()).compare(startDate) < 0 ||
				(dateSs.str()).compare(endDate) > 0)
				answer = true;
		else if(startDate.compare(endDate) > 0)
			if((dateSs.str()).compare(startDate) > 0 ||
				(dateSs.str()).compare(endDate) < 0)
				answer = true;
	}

	if(filterDay == true && answer == false) {
		answer = true;
		while(!matchDays.empty()) {
			if(matchDays.back() != days[ptm->tm_wday])
				matchDays.pop_back();
			else {
				answer = false;
				break;
			}
		}
	}

	if(filterMonth == true && answer == false) {
		answer = true;
		while(!matchMonths.empty()) {
			if(matchMonths.back() != months[ptm->tm_mon])
				matchMonths.pop_back();
			else {
				answer = false;
				break;
			}
		}
	}

	if(filterDbm == true && answer == false) {
		if(minDbm < packet.dbm || maxDbm > packet.dbm)
			answer=true;
	}

	if(uniqMac == true && answer == false) {
		if(uniqMacFirst == true)
			;
	}

	return answer;
}

bool FILTER::setFilter(std::string filter) {
	clearFilter();

	bool syntaxCheck = true;
	std::stringstream ss(filter);
	std::istringstream intConv;
	std::string buff;
	std::string args;
	std::vector<std::string> commands; 

	while(ss >> buff) {
		commands.push_back(buff);
	}

	//Parse each command
	if(commands.size() != 0)
	for(int i = 0; i <= (commands.size() - 1); i++) {
		buff.clear();
		args.clear();
		//Split command into filter type and value
		if((commands[i]).find_first_of("=") == std::string::npos) {
			syntaxCheck = false;
			break;
		}
		else
			for(size_t q = 0; q <= commands[i].find_first_of("=") - 1; q++)
				buff += commands[i].at(q);
			for(int q = commands[i].find_first_of("=") + 1;
				 	q <= ((commands[i]).size()-1); q++)
				args += commands[i].at(q);
		
		//Input values into variables
		if(buff.compare("time" ) == 0) {
			if(args.find_first_of("-")==std::string::npos) {
				syntaxCheck = false;
				break;
			}
			else {
				filterTime = true;
				for(int q = 0; q <= args.find_first_of("-") - 1; q++)
					startTime += args.at(q);
				for(int q = args.find_first_of("-") + 1;
		 				q <= (args.size()-1); q++)
					endTime += args.at(q);
			}
		}
		else if(buff.compare("date") == 0) {
			if(args.find_first_of("-")==std::string::npos) {
				syntaxCheck = false;
				break;
			}
			else {
				filterDate = true;
				for(int q = 0; q <= args.find_first_of("-"); q++)
					startDate += args.at(q);
				for(int q = args.find_first_of("-") + 1;
		 				q <= (args.size()-1); q++)
					endDate += args.at(q);
			}
		}
		else if(buff.compare("days") == 0) {
			filterDay = true;
			for(size_t q = args.find_first_of(",");
					q != std::string::npos;) {
				args[q] = ' ';
				q = args.find_first_of(",", ++q);
			}

			buff.clear();
			ss.str(std::string());
			ss.str(args);

			while(ss >> buff)
				matchDays.push_back(buff);
		}
		else if(buff.compare("months") == 0) {
			filterMonth = true;
			for(size_t q = args.find_first_of(",");
					q != std::string::npos;) {
				args[q] = ' ';
				q = args.find_first_of(",", ++q);
			}

			buff.clear();
			ss.str(std::string());
			ss.str(args);

			while(ss >> buff)
				matchMonths.push_back(buff);
		}
		else if(buff.compare("dbm") == 0) {
			if(args.find_first_of("-") == std::string::npos) {
				syntaxCheck = false;
				break;
			}
			else {
				buff.clear();
				for(int q = 0; q <= args.find_first_of("-") - 1; q++)
					buff += args.at(q);

				intConv.clear();
				intConv.str(buff);
				intConv >> minDbm;

				buff.clear();
				for(int q = args.find_first_of("-") + 1;
		 				q <= (args.size()-1); q++)
					buff += args.at(q);

				intConv.clear();
				intConv.str(buff);
				intConv >> maxDbm;
			}
			if(minDbm > maxDbm)
				syntaxCheck = false;
			else {
				minDbm *= - 1;
				maxDbm *= - 1;
				filterDbm = true;
			}
		}
		else if(buff.compare("uniqMac") == 0) {
				uniqMac = true;
				if(args.compare("first") == 0)
					uniqMacFirst = true;
				else if(args.compare("last") == 0)
					uniqMacLast = true;
				else {
					syntaxCheck = false;
					break;
				}
		}
		else {
			syntaxCheck = false;
			break;
		}
		buff.clear();
		args.clear();
	}
	return syntaxCheck;
}

void FILTER::clearFilter() {
	filterTime = false;
		startTime.clear();
		endTime.clear();

	filterDate = false;
		startDate.clear();
		endDate.clear();

	filterDay = false;
		matchDays.clear();

	filterMonth = false;
		matchMonths.clear();

	filterDbm = false;
		maxDbm = 0;
		minDbm = 0;

	uniqMac = false;
		uniqMacFirst = false;
		uniqMacLast = false;
}