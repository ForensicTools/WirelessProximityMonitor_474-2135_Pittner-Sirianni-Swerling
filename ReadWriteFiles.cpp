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
//void ReadWrite::readFromFile(const char* in_file_name, std::list<packet_structure> *filelist)
//void ReadWrite::readFromFile(const char* in_file_name)
void ReadWrite::readFromFile(const char* in_file_name, std::list<packet_structure> *filelist)
{
	struct packet_structure packet_in;	// struct object called "packet_in"
	Jzon::Array read_capture;			// Jzon array object called "read_capture"

	// calls Jzon file reading function (ReadFile) and passes the parameters "in_file_name" and the read_capture array
	Jzon::FileReader::ReadFile(in_file_name, read_capture.AsArray());

	// for loop goes through array of packets
	for(Jzon::Array::iterator p = read_capture.begin(); p != read_capture.end(); p++)
	{
		Jzon::Object packet = (*p).AsObject();		// creates a new Jzon object called packet which contains the packet data.

		for(Jzon::Object::iterator x = packet.begin(); x != packet.end(); x++) 
		{
			string name = (*x).first;	// "name" holds the field name of data value
			Jzon::Node &node = (*x).second;	// "&node" holds the data value for each field to be stored
			if(node.IsValue())
			{
				// switch statement used to determine which value to store in each field in the packet struct
				switch(node.AsValue().GetValueType())
				{
					// if the value is a string then store in mac field
					case Jzon::Value::VT_STRING: 
						if(name == "MACAddress")
						{
							packet_in.mac = node.ToString();
						}
						break;
					// if the value is a number, then determine if it is epoch time or signal strength
					case Jzon::Value::VT_NUMBER: 
						// if the field name is "Date/Time" then store in epoch_time
						if(name == "EpochTime")
						{
							packet_in.epoch_time = node.ToInt();
						}
						// if the field name is "Signal Strength" then store in dbm
						else if(name == "SignalStrength")
						{
							packet_in.dbm = node.ToFloat();
						}
						break;
				}
			}
		}
		// push packet struct onto list
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
	//Jzon::Object capture_file;
	Jzon::Object packet_data;	// create Jzon object called "packet_data"
	Jzon::Array packet_array;	// create Jzon array to store packets
	string mac_address;		// "mac_address" string holds MAC Address of packet
	int epoch_time;		// "epoch_time" int holds time in seconds of packet timestamp
	int signal_strength;	// "signal_strength" int holds the signal strength of packet

	//create and open file for chart input
	ofstream chartFile;
	ofstream file("chart/input.csv", ios::trunc);
	chartFile.open("chart/input.csv");
	bool first = true;

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
		
		//write to chart input
		if(first){
			//chartFile.Clear();
			chartFile << mac_address << ", " << signal_strength;
			first = false;
		}
		else{
			chartFile << "\n" << mac_address << ", " << signal_strength;
		}

		// adds each value to appropriate field in packet_data object
		packet_data.Add("MACAddress", mac_address);
		packet_data.Add("EpochTime", epoch_time);
		packet_data.Add("SignalStrength", signal_strength);

		packet_array.Add(packet_data);
		// writes each packet to file
	}
	chartFile.close();

	//capture_file.Add(packet_array);
	Jzon::FileWriter::WriteFile(out_file_name, packet_array, Jzon::StandardFormat);
}
