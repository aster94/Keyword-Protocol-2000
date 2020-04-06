/*
KWP2000.h

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

/*  TODO LIST
check types
pids from manzo / ecuhacking / forum ?
make a two dimensional array to save errors?
remove resetError?
in keepalive should i try to send a request anyway instead of closing?
check if the ecu is connected before some functions
check if all serial have F
check verbose/default levels
esp32 can't drive the pin
*/

#ifndef KWP2000_h
#define KWP2000_h

/**
 * @brief You need to pass to the constructor one of these
 */
enum brand
{
  SUZUKI,
  KAWASAKI,
  YAMAHA,
  HONDA
};

/**
 * @brief Some model from the same brand could have different request codes or index for the bytes
 */
enum model
{
  NONE
};

/**
 * @brief Collection of possible debug levels
 */
enum debug_enum
{
  DEBUG_LEVEL_NONE,
  DEBUG_LEVEL_DEFAULT,
  DEBUG_LEVEL_VERBOSE
};

/**
 * @brief Used by `readTroubleCodes()`
 */
enum trouble_code
{
  READ_TOTAL,
  READ_ONLY_ACTIVE,
  READ_ALL
};

class KWP2000
{
public:
  // CONSTRUCTOR
  KWP2000(HardwareSerial *kline_serial, const uint8_t k_out_pin, const brand brand, const model = NONE);

  // SETUP
  void enableDebug(HardwareSerial *debug_serial, const uint8_t debug_level = DEBUG_LEVEL_DEFAULT, const uint32_t debug_baudrate = 115200);
  void setDebugLevel(const uint8_t debug_level);
  void disableDebug();
  void enableDealerMode(const uint8_t dealer_pin);
  void setDealerMode(const uint8_t dealer_mode);
  bool getDealerMode();
  void use_imperial();
  void use_metric();

  // COMMUNICATION - Basic
  int8_t initKline();
  int8_t stopKline();
  void requestSensorsData();
  void keepAlive(uint16_t time = 0);

  // COMMUNICATION - Advanced
  int8_t handleRequest(const uint8_t to_send[], const uint8_t send_len, const uint8_t try_once = false);
  void readTroubleCodes(const trouble_code which = READ_ONLY_ACTIVE);
  void clearTroubleCodes(const uint8_t code = 0x00);
  //void checkTimingParameter();
  //void accessTimingParameter(const uint8_t read_only = true);
  //void resetTimingParameter();
  //void changeTimingParameter(uint32_t new_atp[], const uint8_t new_atp_len);

  // PRINT and GET
  void printStatus(uint16_t time = 5000);
  void printSensorsData();
  void printLastResponse();
  int8_t getStatus();
  int8_t getError();
  void resetError();
  uint8_t getGPS();
  uint8_t getCLUTCH();
  uint8_t getRPM();
  uint8_t getSPEED();
  uint8_t getTPS();
  uint8_t getSTPS();
  uint8_t getIAP();
  uint8_t getIAT();
  uint8_t getECT();
  float getVOLT();

private:
  // K-Line
  HardwareSerial *_kline;
  brand _brand;
  model _model;
  uint8_t _k_out_pin;
  uint8_t _dealer_pin;
  bool _dealer_enabled = false;
  uint8_t _dealer_mode;
  uint8_t _init_sequence_started = false;
  uint8_t _stop_sequence_started = false;
  uint32_t _start_time = 0;
  uint32_t _elapsed_time = 0;
  uint8_t *_response;
  uint8_t _response_len = 0;
  uint8_t _response_data_start = 0;
  uint8_t _ECU_status = false;
  uint32_t _ECU_error = 0;
  uint8_t _init_phase = 0;

  // k line config
  uint8_t _use_length_byte = true;
  uint8_t _use_target_source_address = true;
  uint8_t _timing_parameter = true; // normal
  //uint16_t ISO_T_IDLE = 0;
  uint8_t ISO_T_P2_MIN = 25;
  uint32_t ISO_T_P2_MAX = 50;
  uint16_t ISO_T_P3_MIN = 55;
  uint32_t ISO_T_P3_MAX = 2000;
  uint32_t ISO_T_P3_mdf = 2000;
  uint16_t ISO_T_P4_MIN = 10; // average between min and max value
  uint16_t _keep_iso_alive = 1500;

  // debug
  HardwareSerial *_debug;
  uint8_t _debug_enabled = false;
  uint32_t _debug_baudrate;
  uint8_t _debug_level = DEBUG_LEVEL_DEFAULT;
  uint32_t _last_status_print = 0;
  uint32_t _last_data_print = 0;
  uint32_t _last_sensors_calculated = 0;
  uint32_t _last_correct_response = 0;
  uint32_t _connection_time = 0;

  // sensors
  bool _use_metric_system = true;

  uint8_t _GPS, _CLUTCH, _TPS, _IAP, _ECT, _STPS, _IAT;
  uint16_t _RPM, _SPEED;
  float _VOLT;

  // functions
  void sendRequest(const uint8_t to_send[], const uint8_t send_len, const uint8_t wait_to_send_all = true, const uint8_t use_delay = true);
  void listenResponse(const uint8_t use_delay = true);
  int8_t checkResponse(const uint8_t response_sent[]);
  void setError(const uint8_t error);
  void clearError(const uint8_t error);
  void configureKline();
  uint8_t calc_checksum(const uint8_t data[], const uint8_t data_len);
  void endResponse(const uint8_t received_checksum);
  void connectionExpired();
  void set_bike_specific_values(const brand brand, const model model);
};

#endif // KWP2000_h
