#ifndef RTHEADER_H
#define RTHEADER_H

#include <stdint.h>

struct ieee80211_radiotap_header {
	uint8_t		it_version;		//Always 0
	uint8_t		it_pad;			//Unused padding
	uint16_t	it_len;			//Entire length of RadioTap header including data
	uint32_t	it_present;		//Fields used
} __attribute__((__packed__));

//Static fields based on current development machine
struct rtapdata {
	uint8_t		rate;
	uint8_t		fhss_hop_set;
	uint8_t		fhss_hop_pattern;
	uint8_t		antenna_signal;
	uint8_t		antenna_noise;
	uint16_t	tx_attenuation;
	uint8_t		db_antenna_signal;
} __attribute__((__packed__));
#endif //RTHEADER_H