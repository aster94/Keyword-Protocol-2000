/*
PIDs.h
Parameter IDs

Copyright (c) 2019 Aster94

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, 
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or 
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

////////////// MODIFY HERE ////////////////

// First choose only one of these motorbikes:
// To choose delete the "//" at the beginning of the line
//#define SUZUKI
//#define KAWASAKI
//#define YAMAYA
//#define HONDA

////////////// DON'T MODIFY UNDER THIS LINE ////////////////
////////////// UNLESS YOU KNOW WHAT YOU ARE DOING ////////////////

const uint8_t format_physical = 0x80;
const uint8_t format_functional = 0xC0; // not supported
const uint8_t format_CARB = 0x40;       // not supported
const uint8_t ECU_addr = 0x12;          // between hex 10 and 17
const uint8_t OUR_addr = 0xF1;          // between hex F0 and FD

const uint8_t start_com[] = {0x81};
const uint8_t stop_com[] = {0x82};

// access timing parameter and timing paramenter indicator
const uint8_t atp_read_limits[] = {0x83, 0x00};
const uint8_t atp_set_default[] = {0x83, 0x01};
const uint8_t atp_read_current[] = {0x83, 0x02};
const uint8_t atp_set_given[] = {0x83, 0x03};

const uint8_t tester_present_with_answer[] = {0x3E, 0x01};    // the ECU will ansker
const uint8_t tester_present_without_answer[] = {0x3E, 0x02}; // the ECU won't answer

const uint8_t trouble_codes_all[] = {0x13};
const uint8_t trouble_codes_only_active[] = {0x17};
const uint8_t trouble_codes_with_status[] = {0x18};

const uint8_t clear_trouble_codes[] = {0x14};

#define request_ok(x) x | 0x40
const uint8_t request_rejected = 0x7F;

#if defined(SUZUKI)
#if defined(BIKE_CHOOSED)
#error "two or more bikes choosed, please see src/PIDs.h"
#else
#define BIKE_CHOOSED
#endif // BIKE_CHOOSED

const uint8_t request_sens[] = {0x21, 0x08};

#define PID_SPEED 16
#define PID_RPM_H 17 // high byte
#define PID_RPM_L 18 // low byte
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

//from mark's (ciclegadget) PID_STP 46
#endif // SUZUKI

#if defined(KAWASAKI)
#if defined(BIKE_CHOOSED)
#error "two or more bikes choosed, please see src/PIDs.h"
#else
#define BIKE_CHOOSED
#endif // BIKE_CHOOSED

const uint8_t request_sens[][4] = {
    {0, 1, 2, 3},  //Speed
    {4, 5, 6, 7},  //RPM
    {8, 9, 10, 11} //GEAR
    //todo
};

#define PID_SPEED 16
#define PID_RPM_H 17 // high byte
#define PID_RPM_L 18 // low byte
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

#endif // KAWASAKI

#if defined(YAMAYA)
#if defined(BIKE_CHOOSED)
#error "two or more bikes choosed, please see src/PIDs.h"
#else
#define BIKE_CHOOSED
#endif // BIKE_CHOOSED
/*
As far as I could understand the YDS (Yamaha Diagnostic Protocol) protocol, it is close to the Honda protocol.
There are no sender-/receiver-adresses or header information. Just the checksum at the end.
You initialize the diagnostic mode by sending 0x80 and from then on you can just submit 0x02 and it gives you:
Rpm, Speed, Error, Gear & Checksum.
todo: need more info
*/
#endif // YAMAYA

#if defined(HONDA)
#if defined(BIKE_CHOOSED)
#error "two or more bikes choosed, please see src/PIDs.h"
#else
#define BIKE_CHOOSED
#endif // BIKE_CHOOSED
// see above
#endif // HONDA

#ifndef BIKE_CHOOSED
#error "no bike selected, please see src/PIDs.h"
#endif // BIKE_CHOOSED
