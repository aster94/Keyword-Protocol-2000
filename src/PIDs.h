#define GSXR_600_2011 
/*

#define GSXR_750_2011
#define GSXR_1000_2011

#define GSXR_600_2012
#define GSXR_750_2012
#define GSXR_1000_2012

#define NINJA_636_2011
#define NINJA_1000_2011
*/

//pids


#if defined(GSXR_600_2011)
	uint8_t format = 0x80;
	const uint8_t ECUaddr = 0x12;
	const uint8_t TESTERaddr = 0xF1;

	const uint8_t startCom[] = {0x81, 0x12, 0xF1, 0x81, 0x05};
	const uint8_t startCom_ok[] = {0x80, 0xF1, 0x12, 0x03, 0xC1, 0xEA, 0x8F, 0xC0};
	const uint8_t requestSens[] = {0x80, 0x12, 0xF1, 0x02, 0x21, 0x08, 0xAE};
	//no closeCom?

	#define PID_SPEED 16
	#define PID_RPM_H 17  //high byte
	#define PID_RPM_L 18  //low byte
	#define PID_TPS 19
	#define PID_IAP 20
	#define PID_ECT 21
	#define PID_IAT 22
	#define PID_AP 23
	#define PID_BATT 24
	#define PID_HO2 25
	#define PID_GPS 26
	//31 to 38 FUEL 1 2 3 H and L
	//41 to 44 ignition IGN
	#define PID_STPS 47
	#define PID_PAIR 51
	#define PID_CLUTCH 52
	#define PID_GEAR_3 53

#elif defined(GSXR_750_2011)
	const uint8_t startCom[] = {0x81, 0x12, 0xF1, 0x81, 0x05};
	const uint8_t startCom_ok[] = {0x80, 0xF1, 0x12, 0x03, 0xC1, 0xEA, 0x8F, 0xC0};
	const uint8_t requestSens[] = {0x80, 0x12, 0xF1, 0x02, 0x21, 0x08, 0xAE};

	#define PID_SPEED 17
	

#elif defined(GSXR_1000_2011)
	#define PID_SPEED 18

#elif defined(GSXR_1000_2008)
//from mark's (ciclegadget)
	#define PID_STP 46  

#elif defined(NINJA)

	const uint8_t closeCom[] = {1};
	const uint8_t closeCom_ok[] = {1};


#else
	#error "no bike selected"
#endif