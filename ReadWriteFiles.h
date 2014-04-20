#ifndef READWRITEFILES_H
#define READWRITEFILES_H

#include <string>
#include <time.h>
#include <list>

using namespace std;

/* 
ReadWrite class contains function to read and write packet captures from 
JSON formatted files and also contains a struct and list of structs used
to handle information for each packet
*/
class ReadWrite
{
public:
	// Packet struct contains outline for packet format
	struct Packet
	{
		string mac;		// source mac address
		time_t date_time;	// timestamp of packet
		int sig_str;		// signal strength of device
	};
	
	// creats a list of Packet structs 	
	list<Packet> pcap;
		
	// read function reads in the JSON formatted data from file <in_file_name>
	void readFromFile(const char* in_file_name);
	
	// write function writes the JSON formatted data to file <out_file_name>
	void writeToFile(const char* out_file_name, list<Packet> capture);
		
};
#endif