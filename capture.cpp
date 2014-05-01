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


#include "capture.h"

static const int present_field_align[5] = {8,1,1,4,2};

Capture::Capture(std::string devName)
{	
	errorStr = "";

	handler = pcap_create(devName.c_str(), errbuff); //Creates handle for device

	if (handler == NULL) 			//Ensures capture handle was created
	{
		errorStr = "Capture handle not created";
	}
	
	if(pcap_set_rfmon(handler,1)!=0) //Sets and checks monitor mode
	{
		errorStr = "Monitor mode not enabled";
	}

	//Set capture settings
	pcap_set_snaplen(handler, 128);	//Packet buffer size
	pcap_set_promisc(handler, 0);	//Promiscuous mode 0=off
	pcap_set_timeout(handler, 512);	//Time waiting for packets in ms
	pcap_set_buffer_size(handler, 8192);

	if(pcap_activate(handler) != 0) { //Check for successful handle activation
		errorStr = "Handle not activated";
	}
}

Capture::~Capture()
{
}

packet_structure Capture::CapturePacket() {

	db_offset_size = 0;	//RadioTap offset value

	//Retrieves the next packet
	packet = pcap_next(handler, &header);

	//RadioTap header
	struct ieee80211_radiotap_header *rh_ptr 
		= (struct ieee80211_radiotap_header *) (packet);
	//BitMask fields
	std::bitset<32> present_fields (rh_ptr->it_present);

	//Find offset for the dB data
	for (int i = 0; i <= 4; i++)
		if (present_fields[i] == 1)
			db_offset_size += present_field_align[i];

	//There is always a buffer for "Rate" field
	if(present_fields[2] == 0)
		db_offset_size++;

	//dBm value
	uint8_t	*db_antenna_signal 
		= (uint8_t *)(packet + RADIOTAP_HEADER_SIZE + db_offset_size);


	//802.11 Header
	struct ieee80211_mac_header *mh_ptr = 
		(struct ieee80211_mac_header *) (packet + (*rh_ptr).it_len); //802.11 Header

	//Time value
	packet_data.epoch_time = header.ts.tv_sec;

	//MAC column - MAC addr Changes based on flags present
	if(mh_ptr->fromDS == 0) {
		packet_data.mac = convertArray(mh_ptr->addr2);
	}
	else if(mh_ptr->toDS == 0) {
		packet_data.mac = convertArray(mh_ptr->addr3);
	}
	else {
		packet_data.mac = convertArray(mh_ptr->addr4);
	}
	std::transform( (packet_data.mac).begin(), (packet_data.mac).end(),
		(packet_data.mac).begin(), ::toupper);

	//Signal strength column
	packet_data.dbm = *db_antenna_signal;
	packet_data.dbm -= 256;

	//Time value
	packet_data.epoch_time = header.ts.tv_sec;
	//pcap_close(handler);
	
	return(packet_data);
}

std::string Capture::convertArray(uint8_t addr[6]) {
	std::stringstream ss;
	ss << std::hex << std::setfill('0');

	for(int i = 0; i <= 5; i++) {
		if(i!=0) {
			ss << ":";
		}
		ss << std::setw(2) << static_cast<unsigned>(addr[i]);
	} 
	return ss.str();
}

std::string Capture::captureError(void) {
	return errorStr.c_str();
}