#include "capture.h"

static const int present_field_align[5] = {8,1,1,4,2};

Capture::Capture()
{	
	packet = NULL;
	handler = pcap_create("wlan0",errbuff); //Creates handle for device

	if (handler == NULL) 			//Ensures capture handle was created
	{
		exit(1);
	}
	
	if(pcap_set_rfmon(handler,1)!=0) //Sets and checks monitor mode
	{
		exit(1);
	}

	//Set capture settings
	pcap_set_snaplen(handler, 1500);	//Packet buffer size
	pcap_set_promisc(handler, 0);	//Promiscuous mode 0=off
	pcap_set_timeout(handler, 512);	//Time waiting for packets in ms

	if(pcap_activate(handler) != 0) { //Check for successful handle activation
		exit(1);
	}

	//Begin Channel rotation
	//pthread_create(&chanThread, NULL, &CaptureThread, NULL);
}

Capture::~Capture()
{
}

packet_structure Capture::CapturePacket() {
	while (1) {
	db_offset_size = 0;	//RadioTap offset value

	//Retrieves the next packet
	packet = pcap_next(handler, &header);

	//RadioTap header
	struct ieee80211_radiotap_header *rh_ptr 
		= (struct ieee80211_radiotap_header *) (packet);
	//BitMask fields
	std::bitset<32> present_fields (rh_ptr->it_present);

	//Find offset for the dB data
	for (int i = 0; i <= 4; i++) {
		if (present_fields[i] == 1) {
			db_offset_size += present_field_align[i];
		}
	}

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
		copyArray(packet_data.addr, mh_ptr->addr2);
	}
	else if(mh_ptr->toDS == 0) {
		copyArray(packet_data.addr, mh_ptr->addr3);
	}
	else {
		copyArray(packet_data.addr, mh_ptr->addr4);
	}

	//Signal strength column
	packet_data.dbm = *db_antenna_signal;
	packet_data.dbm -= 256;

	//Time value
	packet_data.epoch_time = header.ts.tv_sec;

	return(packet_data);
	}
}

void Capture::copyArray(uint8_t copy[6], uint8_t orig[6]) {
	for(int i = 0; i <= 5; i++) {
		copy[i] = orig[i];
	}
}