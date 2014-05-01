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


#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <stdio.h>
#include "Jzon.h"
#include <time.h>
#include <list>
#include "ReadWriteFiles.h"

using namespace std;

/*
Name:  readFromFile
Parameter(s):  in_file_name
Purpose:  read in JSON formatted packet data from a file,
store data from each packet in a struct, and create a list
of Packet structs.
*/
void ReadWrite::readFromFile(const char* in_file_name, std::list<packet_structure> *filelist)
{
	struct packet_structure packet_in;	// struct object called "packet_in"
	Jzon::Object capture;	// create Jzon object called "capture"
	
	// calls Jzon file reading function (ReadFile) and passes the parameters "in_file_name" and the capture object
	Jzon::FileReader::ReadFile(in_file_name, capture);

	// for loop goes through each packet in the file
	for(Jzon::Object::iterator it = capture.begin(); it != capture.end();)
	{	
		// for loop to go through and store each value of packet in a struct
		for(int i=0; i<3; i++)
		{
			string name = (*it).first;	// "name" holds the field name of data value
			Jzon::Node &node = (*it).second;	// "&node" holds the data value for each field to be stored
			if(node.IsValue())
			{
				// switch statement used to determine which value to store in each field in the packet struct
				switch(node.AsValue().GetValueType())
				{
					// if the value is a string then store in mac field
					case Jzon::Value::VT_STRING: 
						if(name == "MAC Address")
						{
							packet_in.mac = node.ToString();
						}
						break;
					// if the value is a number, then determine if it is epoch time or signal strength
					case Jzon::Value::VT_NUMBER: 
						// if the field name is "Date/Time" then store in epoch_time
						if(name == "Date/Time")
						{
							packet_in.epoch_time = node.ToInt();
						}
						// if the field name is "Signal Strength" then store in dbm
						else if(name == "Signal Strength")
						{
							packet_in.dbm = node.ToFloat();
						}
						break;
				}
			}
		//increment packet iterator
		it++;
		}
		// push the packet struct onto back of filelist
		filelist->push_back(packet_in);
	}
}

/*
Name:  writeToFile
Parameter(s):  const char* out_file_name, list<Packet> capture
Purpose:  takes a list of Packet structs that are received, 
loops through the list and reads the data for each packet, and 
writes the packet data to a file in JSON format.
*/			
void ReadWrite::writeToFile(const char* out_file_name, list<packet_structure> capture)
{
	Jzon::Object packet_data;	// create Jzon object called "packet_data"
	string mac_address;		// "mac_address" string holds MAC Address of packet
	int epoch_time;		// "epoch_time" int holds time in seconds of packet timestamp
	int signal_strength;	// "signal_strength" int holds the signal strength of packet

	// create list of packet structs called "p"
	list<packet_structure>::reverse_iterator p;

	// for loop goes through the capture and writes each packet to a file
	for(p = capture.rbegin(); p != capture.rend(); p++)
	{
		// clears packet_data object values
		packet_data.Clear();
		
		mac_address = p->mac;	// stores "mac" value in "mac_address"
		epoch_time = p->epoch_time;	// stores "epoch_time" value in "epoch_time"
		signal_strength = p->dbm;	// stores "dbm" value in "signal_strength"
		
		// adds each value to appropriate field in packet_data object
		packet_data.Add("MAC Address", mac_address);
		packet_data.Add("Date/Time", epoch_time);
		packet_data.Add("Signal Strength", signal_strength);

		// writes each packet to file
		Jzon::FileWriter::WriteFile(out_file_name, packet_data, Jzon::StandardFormat);
	}
}