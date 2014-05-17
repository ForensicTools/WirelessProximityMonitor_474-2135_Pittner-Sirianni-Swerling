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


#include "filter.h"

//Static lists of user input strings
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

bool FILTER::filterPacket(struct packet_structure packet,
				std::list<packet_structure> *packetList) {

	//Default answer is don't filter packet
	bool answer = false;

	//Structures for date and time
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

		//Checks if packet time within specified filter range
		if(startTime.compare(endTime) < 0)
			if((timeSs.str()).compare(startTime) < 0 ||
				(timeSs.str()).compare(endTime) > 0)
				answer = true;

		//If start time is greater than end
			//It must extend into next day
		else if(startTime.compare(endTime) > 0)
			if((timeSs.str()).compare(startTime) > 0 ||
				(timeSs.str()).compare(endTime) < 0)
				answer = true;
		else {
			if((timeSs.str()).compare(startTime) != 0)
				answer = true;
		}
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

		//Checks if it is with in the allowable filter date
		if(startDate.compare(endDate) < 0)
			if((dateSs.str()).compare(startDate) < 0 ||
				(dateSs.str()).compare(endDate) > 0)
				answer = true;
		else if(startDate.compare(endDate) > 0)
			if((dateSs.str()).compare(startDate) > 0 ||
				(dateSs.str()).compare(endDate) < 0)
				answer = true;
		else
			if((dateSs.str()).compare(startDate) != 0)
				answer = true;
	}

	if(filterDay == true && answer == false) {
		//Sets fiter to true
		answer = true;
		for(int i = 0; i <= (matchDays.size() - 1); i++) {
			//If packet day matches an allowed day
			//Filter is set back to false
			if(matchDays[i] == days[ptm->tm_wday]) {
				answer = false;
				break;
			}
		}
	}

	if(filterMonth == true && answer == false) {
		//Sets filter to true
		answer = true;
		for(int i = 0; i <= (matchMonths.size()-1); i++) {
			//If packet month matches an allowed month
			//Filter is set back to true
			if(matchMonths[i] == months[ptm->tm_mon]) {
				answer = false;
				break;
			}
		}
	}

	if(filterDbm == true && answer == false) {
		//Checks if dBm is within allowable filter rnage
		if(minDbm < packet.dbm || maxDbm > packet.dbm)
			answer=true;
	}

	if(uniqMac == true && answer == false) {
		//Checks packet MAC against all packets in the display filter list
		for(std::list<packet_structure>::iterator it = packetList->begin(); 
			it != packetList->end(); it++) {
			if((it->mac).compare(packet.mac) == 0) {
				answer = true;
				break;
			}
		}
	}

	if( (blackList == true || whiteList == true) && answer == false) {
		if(whiteList == true)
			answer = true;

		//Checks packet MAC against all packets in the MAC list
		for(std::list<packet_structure>::iterator it = macList.begin(); 
			it != macList.end(); it++) {
			if((it->mac).compare(packet.mac) == 0) {
				answer = !answer; //Tricky thinking
				break;
			}
		}
	}

	if( macMono == true && answer == false) {
		if((macAddr).compare(packet.mac) != 0)
			answer = true;
	}
	
	return answer;
}

bool FILTER::setFilter(std::string filter) {
	//Clears variables before setting them
	clearFilter();

	//Initializes Syntax as good
	bool syntaxCheck = true;
	std::stringstream ss(filter); //Separates commands by spaces
	std::istringstream intConv; //Used for converting string numbers to type int
	std::string buff; //General string buffer
	std::string args; //Holds arguments for commandsf
	std::vector<std::string> commands; //Holds all commands

	//Separates commands by spaces and inputs them into list
	while(ss >> buff) {
		commands.push_back(buff);
	}

	//Parse each command
	if(commands.size() != 0)
	for(int i = 0; i <= (commands.size() - 1); i++) {
		buff.clear();
		args.clear();
		
		//Initial syntax check for '='; All commands should have this
		if((commands[i]).find_first_of("=") == std::string::npos) {
			syntaxCheck = false;
			break;
		}
		else //Split command into filter type and value
			for(size_t q = 0; q <= commands[i].find_first_of("=") - 1; q++)
				buff += commands[i].at(q);
			for(int q = commands[i].find_first_of("=") + 1;
				 	q <= ((commands[i]).size()-1); q++)
				args += commands[i].at(q);
		
		//Input values into variables
		if(buff.compare("time" ) == 0) {
			//Synatx check
			if(args.find_first_of("-")==std::string::npos) {
				syntaxCheck = false;
				break;
			}
			else { //Split start and end time
				filterTime = true;
				for(int q = 0; q <= args.find_first_of("-") - 1; q++)
					startTime += args.at(q);
				for(int q = args.find_first_of("-") + 1;
		 				q <= (args.size()-1); q++)
					endTime += args.at(q);
			}
		}
		else if(buff.compare("date") == 0) {
			//Syntax check
			if(args.find_first_of("-")==std::string::npos) {
				syntaxCheck = false;
				break;
			}
			else { //Split start and end date
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
			//Relplaces commas with spaces
			for(size_t q = args.find_first_of(",");
					q != std::string::npos; q = args.find_first_of(",", ++q)) {
				args[q] = ' ';
			}

			buff.clear();
			ss.clear();
			ss.str(args);

			//Puts days into vector based on spaces
			while(ss >> buff)
				matchDays.push_back(buff);
		}
		else if(buff.compare("months") == 0) {
			filterMonth = true;
			//Replaces commas with spaces
			for(size_t q = args.find_first_of(",");
					q != std::string::npos; q = args.find_first_of(",", ++q)) {
				args[q] = ' ';
			}

			buff.clear();
			ss.clear();
			ss.str(args);
			//Puts days into vector based on spaces
			while(ss >> buff)
				matchMonths.push_back(buff);
		}
		else if(buff.compare("dbm") == 0) {
			//Inital syntax check
			if(args.find_first_of("-") == std::string::npos) {
				syntaxCheck = false;
				break;
			}
			else { 
				buff.clear();

				//Gets Min dBm value
				for(int q = 0; q <= args.find_first_of("-") - 1; q++)
					buff += args.at(q);

				intConv.clear();
				intConv.str(buff);
				intConv >> minDbm; //Convert to int

				buff.clear();

				//Gets Max dBm value
				for(int q = args.find_first_of("-") + 1;
		 				q <= (args.size()-1); q++)
					buff += args.at(q);

				intConv.clear();
				intConv.str(buff);
				intConv >> maxDbm; //Convert to int
			}

			//Syntax check for Min lt Max
			if(minDbm > maxDbm)
				syntaxCheck = false;
			else { //Converts numbers to negative
				minDbm *= - 1;
				maxDbm *= - 1;
				filterDbm = true;
			}
		}
		else if(buff.compare("uniqMac") == 0) {
			//Syntax checks
			if(args.compare("true") == 0)
				uniqMac = true;
			else if(args.compare("false") != 0) {
				syntaxCheck = false;
				break;
			}
		}
		else if(buff.compare("blackList") == 0 || buff.compare("whiteList") == 0) {
			ReadWrite open;
			open.readFromFile(args.c_str(), &macList);

			if( buff.compare("blackList") == 0 )
				blackList=true;
			else
				whiteList=true;

			if( blackList == true && whiteList == true) {
				syntaxCheck = false;
				break;
			}
		}
		else if(buff.compare("mac") == 0) {
			macMono = true;
			macAddr = args.c_str();
		}
		else {
			syntaxCheck = false;
			break;
		}
	}

	return syntaxCheck;
}

void FILTER::clearFilter() {
	//Clear variables & sets filter checks to false
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

	macMono = false;
		macAddr.clear();

	blackList = false;
	whiteList = false;
		macList.clear();
}