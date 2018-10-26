/*
PIDs.h
Parameter IDs

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
/*
$78 requestCorrectlyReceivedResponsePending
This response code shall only be used by a server in case it
cannot send a positive or negative response message based on the client's request message
within the active P2 timing window. This response code shall manipulate the P2max timing
parameter value in the server and the client. The P2max timing parameter is set to the value (in
ms) of the P3max timing parameter. The client shall remain in the receive mode. The server(s)
shall send multiple negative response messages with the negative response code $78 if required.
As soon as the server has completed the task (routine) initiated by the request message it shall
send either a positive or negative response message (negative response message with a
response code other than $78) based on the last request message received. When the client has
received the response message which has been preceded by the negative response message(s)
with response code $78, the client and the server shall reset the P2max timing parameter to the
previous timing value
*/


const uint8_t format_physical = 0x80;
const uint8_t format_functional = 0xC0; // not supported
const uint8_t format_CARB = 0x40;       // not supported
const uint8_t ECU_addr = 0x12;  // between hex 10 and 17
const uint8_t OUR_addr = 0xF1; // between hex F0 and FD

const uint8_t start_com[] = {0x81};
const uint8_t stop_com[] = {0x82};

// access timing parameter and timing paramenter indicator
const uint8_t atp_read_limits[] = {0x83, 0x00};
const uint8_t atp_set_default[] = {0x83, 0x01};
const uint8_t atp_read_current[] = {0x83, 0x02};
const uint8_t atp_set_given[] = {0x83, 0x03};

#define request_ok(x) x | 0x40
const uint8_t request_rejected = 0x7F;

#define GSXR //mdf

// First choose only one of these motorbikes:
// to choose delete the "//"
//#define GSXR      // GSX-R600 and 1000
//#define NINJA   // 636 and 1000
//#define CBR     // CBR600 and 1000
//#define R       // R6 and R1

#if defined(GSXR)
#if defined(BIKE_CHOOSED)
#error "one or more bike choosed, please see KWP2000/src/PIDs.h"
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
#endif // GSXR

#if defined(NINJA)
#if defined(BIKE_CHOOSED)
#error "one or more bike choosed, please see KWP2000/src/PIDs.h"
#else
#define BIKE_CHOOSED
#endif // BIKE_CHOOSED

const uint8_t request_sens[][] = {
    {0, 1, 2, 3},  //Speed
    {4, 5, 6, 7},  //RPM
    {8, 9, 10, 11} //GEAR
    //todo
};

#endif // NINJA

#if defined(R)
#if defined(BIKE_CHOOSED)
#error "one or more bike choosed, please see KWP2000/src/PIDs.h"
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
#endif // R

#if defined(CBR)
#if defined(BIKE_CHOOSED)
#error "one or more bike choosed, please see KWP2000/src/PIDs.h"
#else
#define BIKE_CHOOSED
#endif // BIKE_CHOOSED
// see above
#endif // CBR

#ifndef BIKE_CHOOSED
#error "no bike selected, please see KWP2000/src/PIDs.h"
#endif // BIKE_CHOOSED
