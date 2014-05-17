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


#ifndef READWRITEFILES_H
#define READWRITEFILES_H

#include <string>
#include <time.h>
#include <list>
#include "PacketStructure.h"

using namespace std;

/* 
ReadWrite class contains function to read and write packet captures from 
JSON formatted files and also contains a struct and list of structs used
to handle information for each packet
*/
class ReadWrite
{
public:
	// read function reads in the JSON formatted data from file <in_file_name>
	void readFromFile(const char* in_file_name, std::list<packet_structure> *filelist);
	
	// write function writes the JSON formatted data to file <out_file_name>
	void writeToFile(const char* out_file_name, std::list<packet_structure> capture);
};
#endif