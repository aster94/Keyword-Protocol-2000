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
pids from wikipedia / manzo / ecuhacking / forum ?
how to manage model/brand?
make an array to save errors?
add F before all serial buffer
check debug levels >
remove resetError?
remember to update keywords.txt and github
*/

#ifndef KWP2000_h
#define KWP2000_h

enum debug_enum // debug levels
{
    DEBUG_LEVEL_NONE,
    DEBUG_LEVEL_DEFAULT,
    DEBUG_LEVEL_VERBOSE
};

class KWP2000
{
  public:
    // CONSTRUCTOR
    KWP2000(HardwareSerial *kline_serial, const uint8_t k_out_pin, const uint32_t kline_baudrate = 10400);

    // SETUP
    void enableDebug(HardwareSerial *debug_serial, const uint32_t debug_baudrate = 115200, const uint8_t debug_level = DEBUG_LEVEL_DEFAULT);
    void setDebugLevel(const uint8_t debug_level);
    void disableDebug();
    void enableDealerMode(const uint8_t dealer_pin);
    void dealerMode(const uint8_t dealer_mode);

    // COMMUNICATION - Basic
    int8_t initKline(uint8_t **p_p = nullptr);
    int8_t stopKline(uint8_t **p_p = nullptr, uint8_t *p_p_len = nullptr);
    void keepAlive(uint16_t time = 1000);
    void requestSensorsData();

    // COMMUNICATION - Advanced
    void sendRequest(const uint8_t to_send[], const uint8_t send_len, const uint8_t wait_to_send_all = false, const uint8_t use_delay = false);
    void listenResponse(uint8_t *resp = nullptr, uint8_t *resp_len = nullptr, const uint8_t use_delay = false);

    // PRINT and GET
    void printStatus(uint16_t time = 2000);
    void printSensorsData(uint16_t time = 1);
    void printLastResponse();
    int8_t getStatus();
    int8_t getError();
    void resetError();
    uint8_t getGPS();
    uint8_t getRPM();
    uint8_t getSPEED();
    uint8_t getTPS();
    uint8_t getIAP();
    uint8_t getIAT();
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
    uint32_t _last_sensors_calculated = 0;
    uint32_t _last_correct_response = 0;
    uint8_t *_pArray;
    uint8_t _pArray_len;
    uint8_t _pArray_is_allocated = false;

    HardwareSerial *_debug;
    uint32_t _debug_baudrate;
    uint8_t _debug_level = DEBUG_LEVEL_NONE;
    uint8_t _ECU_status = false;
    uint32_t _ECU_error = 0;

    uint8_t _GPS, _RPM, _SPEED, _TPS, _IAP, _ECT, _STPS, _IAT;
    uint8_t _GEAR1, _GEAR2, _GEAR3;

    // private functions
    void setError(const uint8_t error);
    uint8_t calc_checksum(const uint8_t data[], const uint8_t len);
    int8_t compareResponse(const uint8_t expectedResponse[], const uint8_t receivedResponse[], const uint8_t expectedResponseLen);
};

#endif
