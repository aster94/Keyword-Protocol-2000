/*
KWP2000.h

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

/*  TODO
check types
pid from manzo / ecuhacking / forum ?
how to manage model/brand?
make an array to save errors?
add F before all serial buffer?
check debug levels
sendrequest protection before init
data array given by the user which superseed _pArray
*/

#ifndef KWP2000_h
#define KWP2000_h

#include "Arduino.h"
const uint8_t maxLen = 60; // maximum lenght of a response from the ecu


// These values are defined by the ISO protocol
#define ISO_BYTE_DELAY 10
#define ISO_MAX_SEND_TIME 2000
#define ISO_DELAY_BETWEN_REQUEST 40
#define ISO_START_TIME 2000
#define ISO_CONNECTION_LOST 2000

#define KEEP_ALIVE_TIME 1000

enum debug_enum // debug level
{
    DEBUG_LEVEL_NONE,
    DEBUG_LEVEL_DEFAULT,
    DEBUG_LEVEL_VERBOSE
};

enum error_enum // a nice collection of ECU Errors
{
    EE_CLEAR,  // clear all errors
    EE_START,  // unable to start comunication
    EE_STOP,   // unable to stop comunication
    EE_TO,     // data is not for us
    EE_FROM,   // data don't came from the ECU
    EE_CS,     // checksum error
    EE_ECHO,   // echo error
    EE_MEMORY, // memory leak
    EE_UNEX,   // unexpected error
    EE_HEADER, // strange header
    EE_STEP,   // request without init
    EE_TOTAL   // this is just to know how many possible errors are in this enum
};

class KWP2000
{
  public:
    // SETUP
    KWP2000(HardwareSerial *kline_serial, const uint8_t k_out_pin, const uint8_t model, const uint32_t kline_baudrate = 10400);

    void enableDebug(HardwareSerial *debug_serial, const uint32_t debug_baudrate = 115200, const uint8_t debug_level = DEBUG_LEVEL_DEFAULT);
    void disableDebug();
    void setDebugLevel(const uint8_t debug_level);
    void enableDealerMode(const uint8_t dealer_pin);
    void dealerMode(const uint8_t dealer_mode);

    // COMMUNICATION
    void sendRequest(const uint8_t to_send[], const uint8_t send_len, const uint8_t wait_to_send_all = true, const uint8_t use_delay = true);
    void listenResponse(uint8_t *resp = nullptr, uint8_t *resp_len = nullptr, const uint8_t use_delay = true);
    void requestSensorsData();
    int8_t initKline();
    int8_t stopKline();
    void keepAlive(uint16_t time = 1000);

    // PRINT and GET
    void printStatus(uint16_t time = 2000);
    void printSensorsData(uint16_t time = 1);
    void printLastResponse(uint8_t *resp = nullptr, uint8_t **resp_len = nullptr);
    int8_t getStatus();
    int8_t getError();
    void resetError(); // todo should i remove this?
    uint8_t getGear();
    uint8_t getRPM();
    uint8_t getSpeed();
    uint8_t getTPS();
    uint8_t getIAP();
    uint8_t getECT();
    uint8_t getSTPS();
    // todo voltage, temperature ecc

  private:
    // declatations:
    HardwareSerial *_kline;
    uint32_t _kline_baudrate;
    uint8_t _k_out_pin;
    uint8_t _model;
    uint8_t _dealer_pin;
    uint8_t _dealer_mode;

    uint8_t _sequence_started = false;
    uint32_t _start_time = 0;
    uint32_t _time_elapsed = 0;
    uint32_t _last_status_print = 0;
    uint32_t _last_data_print = 0;
    uint8_t *_pArray;
    uint8_t _pArray_len;
    uint8_t _pArray_is_allocated = false;

    HardwareSerial *_debug;
    uint32_t _debug_baudrate;
    uint8_t _debug_level = DEBUG_LEVEL_NONE;
    uint8_t _ECU_status = false;
    uint32_t _ECU_error = 0;
    uint32_t _last_correct_response = 0;

    uint8_t _GEAR, _RPM, _SPEED, _TPS, _IAP, _ECT, _STPS, _IAT;
    uint8_t _GEAR1, _GEAR2, _GEAR3;

    // private functions
    void setError(const uint8_t error);
    uint8_t calc_checksum(const uint8_t data[], const uint8_t len);
    int8_t compareResponse(const uint8_t expectedResponse[], const uint8_t receivedResponse[], const uint8_t expectedResponseLen);
};

#endif
