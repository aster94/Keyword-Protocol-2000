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
controllare i tipi
pid dal manzo / ecuhacking / forum ?
gestione modello?
creare un array che memorizza gli errori?
store array? meglio malloc/new?


//working return 1
//errors returns negative numbers
*/

#ifndef KWP2000_h
#define KWP2000_h

#include "Arduino.h"


// These values are defined by the ISO protocol
#define ISO_BYTE_DELAY 10
#define ISO_MAX_SEND_TIME 2000
#define ISO_DELAY_BETWEN_REQUEST 40

// debug level
#define DEBUG_LEVEL_VERBOSE 10
#define DEBUG_LEVEL_DEFAULT 5
#define DEBUG_LEVEL_NONE 0



class KWP2000{
  public:
    int8_t test();

    // config
    KWP2000(HardwareSerial* kline_serial, const uint32_t kline_baudrate, const uint8_t k_out_pin, const uint8_t model);
    void enableDebug(HardwareSerial* debug_serial, const uint32_t debug_baudrate, const uint8_t debug_level = DEBUG_LEVEL_VERBOSE);
    void disableDebug();
    void setDebugLevel(const uint8_t debug_level);
    void enableDealerMode(const uint8_t dealer_pin);
    void dealerMode(const uint8_t dealer_mode);

    //communication
    int8_t initKline();   //make this returning 0 until the time pass
    int8_t stopKline();   //same here
    void requestSensorData();
    int8_t sendRequest(const uint8_t toSend[], const uint8_t sendlen);
    void listenResponse(uint8_t arr[]);//make it returning the pointer to the array?
    void processSensorData(const uint8_t data[], const uint8_t dataLen); //move to private?
    void keepAlive();
    
    //
    void printStatus();
    void printError();
    int8_t getState();
    int8_t getError();
    void resetError();//should i remove this?
    uint8_t getGear();
    uint8_t getRPM();
    uint8_t getSpeed();
    uint8_t getTPS();
    uint8_t getIAP();
    uint8_t getECT();
    uint8_t getSTPS();
    //voltage, temperature ecc

  
  private:

    // declatations:
    HardwareSerial* _kline;
    uint32_t _kline_baudrate;
    uint8_t _k_out_pin;
    uint8_t _model;//useful?
    uint8_t _dealer_pin;

    HardwareSerial* _debug;
    uint32_t _debug_baudrate;
    uint8_t _debug_level = DEBUG_LEVEL_NONE;

    uint8_t _ECUerror = 0;
    //each bit describe an error
    #define EE_START 0  // unable to start comunication
    #define EE_STOP 1   // unable to stop comunication
    #define EE_TO 2     // data is not for us
    #define EE_FROM 3   // data don't came from the ECU
    #define EE_CS 4     // checksum error
    #define EE_ECHO 5   // echo error
    #define EE_FUTURE 6 // future use
    #define EE_FUTURE 7 // future use
    uint8_t _SPEED, _RPM, _TPS, _IAP, _ECT, _IAT, _STPS, _GEAR, _GEAR1, _GEAR2, _GEAR3;


    uint8_t _ECUstate = false;
    uint64_t _lastDataSent;//?
    uint64_t _lastDataRequested;//?

    // private functions
    uint8_t calc_checksum(const uint8_t data[], const uint8_t len);
    int8_t compareResponse (const uint8_t expectedResponse[], const uint8_t receivedResponse[], const uint8_t expectedResponseLen);

};

#endif