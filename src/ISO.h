/*
ISO.h
Parameter IDs

Copyright (c) Aster94

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

#ifndef ISO_h
#define ISO_h

#include "KWP2000.h"

// These values are defined by the ISO protocol
#define ISO_BAUDRATE 10400
#define ISO_MAX_DATA 260 ///< maximum length of a response from the ecu: 255 data + 4 header + 1 checksum
#define ISO_T_P1 10      ///< inter byte time for ECU response - min: 0 max: 20
#define ISO_T_P2_MIN_LIMIT 50
#define ISO_T_P2_MAX_LIMIT 89600 ///< P2 time between tester request and ECU response or two ECU responses
#define ISO_T_P3_MAX_LIMIT 89600 ///< P3 time between end of ECU responses and start of new tester request
#define ISO_T_P4_MAX_LIMIT 20    ///< inter byte time for tester request
// P2 (min & max), P3 (min & max) and P4 (min) are defined by the ECU with accessTimingParameter()
#define ISO_T_IDLE 1000             ///< min 300, max undefined
#define ISO_T_INIL (unsigned int)25 ///< Initialization low time
#define ISO_T_WUP (unsigned int)50  ///< Wake up Pattern

// Common values between all brands/bikes

const uint8_t format_physical = 0x80;
const uint8_t format_functional = 0xC0; // not supported
const uint8_t format_CARB = 0x40;       // not supported

const uint8_t OUR_addr = 0xF1; // between hex F0 and FD

const uint8_t start_com[] = {0x81};
const uint8_t stop_com[] = {0x82};
uint8_t start_diagnostic[2] = {0x10};
const uint8_t read_data_local_identifier = 0x21;

// access timing parameter and timing paramenter indicator
const uint8_t atp_read_limits[] = {0x83, 0x00};
const uint8_t atp_set_default[] = {0x83, 0x01};
const uint8_t atp_read_current[] = {0x83, 0x02};
const uint8_t atp_set_given[] = {0x83, 0x03};

const uint8_t tester_present_with_answer[] = {0x3E, 0x01};    // the ECU will answer
const uint8_t tester_present_without_answer[] = {0x3E, 0x02}; // the ECU won't answer

const uint8_t trouble_codes_all[] = {0x13};
const uint8_t trouble_codes_only_active[] = {0x17};
const uint8_t trouble_codes_with_status[] = {0x18};
const uint8_t clear_trouble_codes[] = {0x14};

#define request_ok(x) x | 0x40
const uint8_t request_rejected = 0x7F;

// Specific brands/bikes values here are only declared and will be assigned inside the function
// this is very likely to change, do not refert too much to the code below

uint8_t ECU_addr;

uint8_t suzuki_request_sens[] = {0x21, 0x08};
uint8_t kawasaki_request_gps[2] = {0x21, 0x0B};
uint8_t kawasaki_request_rpm[2] = {0x21, 0x09};
uint8_t kawasaki_request_speed[2] = {0x21, 0x0c};
uint8_t kawasaki_request_tps[2] = {0x21, 0x04};
uint8_t kawasaki_request_iap[2] = {0x21, 0x07};
uint8_t kawasaki_request_iat[2] = {0x21, 0x05};
uint8_t kawasaki_request_ect[2] = {0x21, 0x06};
uint8_t yamaha_request_sens[2] = {'?'};
uint8_t honda_request_sens[2] = {'?'};

uint8_t IDX_GPS;
uint8_t IDX_CLUTCH;
uint8_t IDX_RPM_H;
uint8_t IDX_RPM_L;
uint8_t IDX_SPEED;
uint8_t IDX_TPS;
uint8_t IDX_STPS;
uint8_t IDX_IAP;
uint8_t IDX_IAT;
uint8_t IDX_ECT;
uint8_t IDX_VOLT;

void KWP2000::set_bike_specific_values(const brand brand, const model model)
{
    if (brand == SUZUKI)
    {
        ECU_addr = 0x12;

        IDX_GPS = 26;
        IDX_CLUTCH = 52;
        IDX_RPM_H = 17;
        IDX_RPM_L = 18;
        IDX_SPEED = 16;
        IDX_TPS = 19;
        IDX_STPS = 46;
        IDX_IAP = 20;
        IDX_IAT = 22;
        IDX_ECT = 21;
        IDX_VOLT = 24;
        /*
        CLT??   20
        HO2??   25
        IAP2??  27
        idle speed?? 28
        ISC valve controls the volume of air that bypasses the throttle valve -29
        Fuel 31-38
        mode (map)?? 50
        pair 51
        */
    }
    else if (brand == KAWASAKI)
    {
        ECU_addr = 0x11;
        start_diagnostic[1] = {0x80};
        IDX_GPS = 6;
        IDX_RPM_H = 6;
        IDX_RPM_L = 7;
        IDX_SPEED = 6;
        IDX_TPS = 6;
        IDX_IAP = 6;
        IDX_IAT = 6;
        IDX_ECT = 6;
    }
    else if (brand == YAMAHA)
    {
        /*
        As far as I could understand the YDS (Yamaha Diagnostic Protocol) protocol, it is close to the Honda protocol.
        There are no sender-/receiver-addresses or header information. Just the checksum at the end.
        You initialize the diagnostic mode by sending 0x80 and from then on you can just submit 0x02 and it gives you:
        Rpm, Speed, Error, Gear & Checksum.
        */
        ECU_addr = '?';
    }
    else if (brand == HONDA)
    {
        // https://gonzos.net/projects/ctx-obd/
        ECU_addr = '?';
    }
}

#endif // ISO_h
