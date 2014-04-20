#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <stdio.h>
#include "Jzon.h"
#include <time.h>
#include <list>

using namespace std;
#include "ReadWriteFiles.h"

/*
Name:  readFromFile
Parameter(s):  in_file_name
Purpose:  read in JSON formatted packet data from a file,
store data from each packet in a struct, and create a list
of Packet structs.
*/
void ReadWrite::readFromFile(const char* in_file_name)
{
	Packet packet_in;	// struct object called "packet_in"
	Jzon::Object capture;	// create Jzon object called "capture"
	
	// calls Jzon file reading function (ReadFile) and passes the parameters "in_file_name" and the capture object
	Jzon::FileReader::ReadFile(in_file_name, capture);

	// for loop goes through each packet in the file
	for(Jzon::Object::iterator it = capture.begin(); it !=capture.end(); ++it)
	{
		string name = (*it).first;
		Jzon::Node &node = (*it).second;
		
		if(node.IsValue())
		{
			switch(node.AsValue().GetValueType())
			{
				case Jzon::Value::VT_STRING: 
					cout << node.ToString();
					packet_in.mac = node.ToString();
					break;
				case Jzon::Value::VT_NUMBER: 
					if(name == "Date/Time")
					{
						time_t time_stamp = node.ToFloat(); 
						cout << time_stamp;
						packet_in.date_time = time_stamp;
					}
					else
					{
						cout << node.ToFloat();
						packet_in.sig_str = node.ToFloat();
					}
					break;
			}
		}
		pcap.push_front(packet_in);
		cout << endl;
	}
}

/*
Name:  writeToFile
Parameter(s):  const char* out_file_name, list<Packet> capture
Purpose:  takes a list of Packet structs that are received, 
loops through the list and reads the data for each packet, and 
writes the packet data to a file in JSON format.
*/			
void ReadWrite::writeToFile(const char* out_file_name, list<Packet> capture)
{
	Jzon::Object packet_data;
	string mac_address;
	int epoch_time;
	int signal_strength;

	list<Packet>::iterator p;

	for(p = capture.begin(); p != capture.end(); p++)
	{
		mac_address = p->mac;
		epoch_time = p->date_time;
		signal_strength = p->sig_str;
		
		packet_data.Add("MAC Address", mac_address);
		packet_data.Add("Date/Time", epoch_time);
		packet_data.Add("Signal Strength", signal_strength);

		Jzon::FileWriter::WriteFile(out_file_name, packet_data, Jzon::StandardFormat);
	}
}