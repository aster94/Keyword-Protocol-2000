/*
KWP2000.cpp

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

#include "Arduino.h"
#include "KWP2000.h"
#include "PIDs.h"
#if defined(ARDUINO_ARCH_ESP32)
#include <new>
#endif

#define maybe 2

//#define FAHRENHEIT
#define TO_FAHRENHEIT(x) x * 1.8 + 32

#define LEN(x) ((sizeof(x) / sizeof(0 [x])) / ((size_t)(!(sizeof(x) % sizeof(0 [x])))))

// These values are defined by the ISO protocol
#define ISO_MAX_DATA 260 // maximum lenght of a response from the ecu: 255 data + 4 header + 1 checksum

// all the times are an average between max and minimum
#define ISO_T_P1 10 // inter byte time for ECU response - min: 0 max: 20
#define ISO_T_P2_MIN_LIMIT 50
#define ISO_T_P2_MAX_LIMIT 89600
#define ISO_T_P3_MAX_LIMIT 89600
// P2 time between tester request and ECU response or two ECU responses
// P3 time between end of ECU responses and start of new tester request
#define ISO_T_P4_MAX_LIMIT 20 //inter byte time for tester request
// P2 (min & max), P3 (min & max) and P4 (min) are defined by the ECU with accessTimingParameter()

// Initialization
#define ISO_T_IDLE_NEW 2000 // min 300, max undefinied
#define ISO_T_INIL 25       // Initialization low time
#define ISO_T_WUP 50        // Wake up Pattern

enum error_enum // a nice collection of ECU Errors
{
    EE_TEST,   // for various test
    EE_START,  // unable to start comunication
    EE_STOP,   // unable to stop comunication
    EE_TO,     // data is not for us
    EE_FROM,   // data don't came from the ECU
    EE_CS,     // checksum error
    EE_ECHO,   // echo error
    EE_MEMORY, // memory leak
    EE_UNEX,   // unexpected error
    EE_HEADER, // strange header
    EE_USER,   // error due to wrong call of a function
    EE_CONFIG, // strange config value in the key bytes
    EE_P3MAX,  // time out communication
    EE_CR,     // check response error
    EE_ATP,    // problem setting the timing parameter
    EE_WR,     // We get a reject for a request we didn't sent
    EE_US,     // not supported, yet
    EE_TOTAL   // this is just to know how many possible errors are in this enum
};

////////////// CONSTRUCTOR ////////////////

KWP2000::KWP2000(HardwareSerial *kline_serial, const uint8_t k_out_pin, const uint32_t kline_baudrate)
{
    _kline = kline_serial;
    _kline_baudrate = kline_baudrate;
    _k_out_pin = k_out_pin;
}

////////////// SETUP ////////////////

void KWP2000::enableDebug(HardwareSerial *debug_serial, const uint32_t debug_baudrate, const uint8_t debug_level)
{
    _debug = debug_serial;
    _debug->begin(debug_baudrate);
    _debug_level = debug_level;
    _debug_enabled = true;

    if (_debug_level >= DEBUG_LEVEL_DEFAULT)
    {
        _debug->println(F("Debug enabled"));
    }
}

void KWP2000::setDebugLevel(const uint8_t debug_level)
{
    _debug_level = debug_level;
    if (_debug_level >= DEBUG_LEVEL_DEFAULT)
    {
        _debug->print(F("Debug level: "));
        _debug->println(debug_level);
    }
}

void KWP2000::disableDebug()
{
    if (_debug_level >= DEBUG_LEVEL_DEFAULT)
    {
        _debug->println(F("Debug disabled"));
    }
    _debug->end();
    _debug_enabled = false;
}

void KWP2000::enableDealerMode(const uint8_t dealer_pin)
{
    _dealer_pin = dealer_pin;
    pinMode(_dealer_pin, OUTPUT);
}

void KWP2000::dealerMode(const uint8_t dealer_mode)
{
    _dealer_mode = dealer_mode;
    digitalWrite(_dealer_pin, _dealer_mode);
    if (_debug_level >= DEBUG_LEVEL_DEFAULT)
    {
        _debug->print(F("Dealer mode: "));
        _debug->println(_dealer_mode == 1 ? "Enabled" : "Disabled");
    }
}

////////////// COMMUNICATION - Basic ////////////////

int8_t KWP2000::initKline(uint8_t **p_p)
{
    if (_ECU_status == true)
    {
        if (_debug_level >= DEBUG_LEVEL_DEFAULT)
        {
            _debug->println(F("\nAlready connected"));
        }
        return 1;
    }

    if (_init_sequence_started == false)
    {
        _init_sequence_started = true;

        if (_debug_level >= DEBUG_LEVEL_DEFAULT)
        {
            _debug->println(F("\nInitialize K-line"));
        }

        if (_response_is_allocated == false)
        {
#if defined(ARDUINO_ARCH_ESP32)
            _response = new (std::nothrow) uint8_t[ISO_MAX_DATA];
#else
            _response = new uint8_t[ISO_MAX_DATA];
#endif
            _response_is_allocated = true;

            if (p_p != nullptr)
            {
                // we passed a pointer which will point to _response
                *p_p = _response;

                if (_debug_level == DEBUG_LEVEL_VERBOSE)
                {
                    _debug->print(F("Allocating given poiners: "));
                }
                if (p_p == nullptr)
                {
                    if (_debug_level == DEBUG_LEVEL_VERBOSE)
                    {
                        _debug->println(F("wrong"));
                    }
                    setError(EE_MEMORY);
                }
                else
                {
                    if (_debug_level == DEBUG_LEVEL_VERBOSE)
                    {
                        _debug->println(F("correct"));
                    }
                }
            }
        }
        else
        {
            if (_debug_level == DEBUG_LEVEL_VERBOSE)
            {
                _debug->println(F("Memory already allocated"));
            }
        }

        if (_response == nullptr)
        {
            if (_debug_level == DEBUG_LEVEL_VERBOSE)
            {
                _debug->println(F("Memory allocation: wrong"));
            }
            setError(EE_MEMORY);
            return -1;
        }
        else
        {
            if (_debug_level == DEBUG_LEVEL_VERBOSE)
            {
                _debug->println(F("Memory allocation: correct"));
            }
        }

        if (ISO_T_IDLE == 0)
        {
            // first attempt to init the k-line
            ISO_T_IDLE = ISO_T_IDLE_NEW;
        }
        else if (bitRead(_ECU_error, EE_P3MAX) == 1)
        {
            // after the connection has been lost due to time out of P3
            ISO_T_IDLE = 100; // should be 0
        }
        else
        {
            // after a stopKline
            ISO_T_IDLE = ISO_T_P3_MAX;
        }

        _use_lenght_byte = false;
        _use_target_source_address = true;
        //_kline->end();
        pinMode(_k_out_pin, OUTPUT);
        digitalWrite(_k_out_pin, LOW);

        _start_time = millis();
        _elapsed_time = 0;
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->println(F("Starting sequence"));
        }
    }
    _elapsed_time = millis() - _start_time;

    if (_elapsed_time < ISO_T_IDLE)
    {
        if (digitalRead(_k_out_pin) != HIGH)
        {
            digitalWrite(_k_out_pin, HIGH);
            if (_debug_level == DEBUG_LEVEL_VERBOSE)
            {
                _debug->print(F("T0:\t"));
                _debug->println(_elapsed_time);
            }
        }
        return 0;
    }
    else if ((_elapsed_time >= ISO_T_IDLE) && (_elapsed_time < ISO_T_IDLE + ISO_T_INIL))
    {
        if (digitalRead(_k_out_pin) != LOW)
        {
            digitalWrite(_k_out_pin, LOW);
            if (_debug_level == DEBUG_LEVEL_VERBOSE)
            {
                _debug->print(F("T1:\t"));
                _debug->println(_elapsed_time);
            }
        }
        return 0;
    }
    else if ((_elapsed_time >= ISO_T_IDLE + ISO_T_INIL) && (_elapsed_time < ISO_T_IDLE + ISO_T_WUP))
    {
        if (digitalRead(_k_out_pin) != HIGH)
        {
            digitalWrite(_k_out_pin, HIGH);
            if (_debug_level == DEBUG_LEVEL_VERBOSE)
            {
                _debug->print(F("T2:\t"));
                _debug->println(_elapsed_time);
            }
        }
        return 0;
    }
    else if (_elapsed_time >= (ISO_T_IDLE + ISO_T_WUP))
    {
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->print(F("T3:\t"));
            _debug->println(_elapsed_time);
            _debug->println(F("\nSending the start sequence"));
        }
        _init_sequence_started = false;

        _start_time = 0;
        _elapsed_time = 0;
        _kline->begin(_kline_baudrate, SERIAL_8O1);

        if (handleRequest(start_com, LEN(start_com)) == true)
        {
            if (_debug_level >= DEBUG_LEVEL_DEFAULT)
            {
                _debug->println(F("ECU connected"));
            }
            _connection_time = millis();
            _ECU_status = true;
            _ECU_error = 0;
            configureKline();
        }
        else
        {
            if (_debug_level >= DEBUG_LEVEL_DEFAULT)
            {
                _debug->println(F("Initialization failed"));
            }
            _ECU_status = false;
            ISO_T_IDLE = 0;
            setError(EE_START);
            return -2;
        }

        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->print(F("Reading timing limits"));
        }
        if (handleRequest(atp_read_limits, LEN(atp_read_limits)) == true)
        {
            accessTimingParameter(true);
        }
        else
        {
            if (_debug_level == DEBUG_LEVEL_VERBOSE)
            {
                _debug->println(F("Error reading limits ATP"));
            }
        }

        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->print(F("Reading current timing paramenters"));
        }

        if (handleRequest(atp_read_current, LEN(atp_read_current)) == true)
        {
            accessTimingParameter();
            return 1; // end of the init sequence
        }
        else
        {
            if (_debug_level == DEBUG_LEVEL_VERBOSE)
            {
                _debug->println(F("Error reading current ATP"));
            }
            setError(EE_ATP);
            return -3;
        }
    }
    //the program should never arrive here
    setError(EE_UNEX);
    return -9;
}

int8_t KWP2000::stopKline(uint8_t **p_p, uint8_t *p_p_len)
{
    if (_ECU_status == false)
    {
        if (_debug_level >= DEBUG_LEVEL_DEFAULT)
        {
            _debug->println(F("\nAlready disconnected"));
        }
        return 1;
    }

    if (_stop_sequence_started == false)
    {
        _stop_sequence_started = true;

        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->println(F("Closing K-line"));
        }

        if (handleRequest(stop_com, LEN(stop_com)) == true)
        {
            //closed without problems
            _ECU_error = 0;
        }
        else
        {
            //not closed correctly
            setError(EE_STOP);
        }

        // we passed only one of these two arguments
        if ((p_p == nullptr) ^ (p_p_len == nullptr))
        {
            setError(EE_USER);
        }

        // deference the pointer if we passed it
        if (p_p != nullptr && p_p_len != nullptr)
        {
            *p_p = nullptr;
            *p_p_len = 0;
            if (_debug_level == DEBUG_LEVEL_VERBOSE)
            {
                _debug->print(F("Deallocating given poiners: "));
            }
            if (*p_p == nullptr)
            {
                if (_debug_level == DEBUG_LEVEL_VERBOSE)
                {
                    _debug->println(F("correct"));
                }
            }
            else
            {
                if (_debug_level == DEBUG_LEVEL_VERBOSE)
                {
                    _debug->println(F("wrong"));
                }
            }
        }

        delete[] _response;
        _response_is_allocated = false;
        _response = nullptr;
        _response_len = 0;
        _response_data_start = 0;

        if (_response == nullptr)
        {
            if (_debug_level == DEBUG_LEVEL_VERBOSE)
            {
                _debug->println(F("_response deallocated"));
            }
        }
        else
        {
            if (_debug_level == DEBUG_LEVEL_VERBOSE)
            {
                _debug->println(F("_response not deallocated"));
            }
            setError(EE_MEMORY);
        }

        _last_correct_response = 0;
        _last_data_print = 0;
        _last_sensors_calculated = 0;
        _last_status_print = 0;
        _connection_time = 0;

        _kline->end();

        _ECU_error = 0;
        _start_time = millis();
        _elapsed_time = 0;
    }

    _elapsed_time = millis() - _start_time;

    if (_elapsed_time < ISO_T_P3_MAX)
    {
        return 0;
    }
    else
    {
        if (_debug_level >= DEBUG_LEVEL_DEFAULT)
        {
            _debug->println(F("ECU disconnected"));
        }
        _ECU_status = false;
        _start_time = 0;
        _elapsed_time = 0;
        _stop_sequence_started = false;
        return 1;
    }
}

int8_t KWP2000::handleRequest(const uint8_t to_send[], const uint8_t send_len)
{
    uint8_t attempt = 1;
    uint8_t completed = false;
    while (attempt <= 3 && completed == false)
    {
        sendRequest(to_send, send_len);
        listenResponse();
        if (checkResponse(to_send) == true)
        {
            completed = true;
        }
        else
        {
            if (_debug_level == DEBUG_LEVEL_VERBOSE)
            {
                _debug->print(F("Attempt "));
                _debug->print(attempt);
                _debug->print(F(" not luckly"));
                _debug->println(attempt < 3 ? ", trying again"
                                            : "\nWe wasn't able to comunicate");
            }
            attempt++;
        }
    }

    if (completed == true)
    {
        return true;
    }
    else
    {
        // we made more than 3 attemps so there is a problem
        return -1;
    }
}

void KWP2000::requestSensorsData()
{
    if (_ECU_status == false)
    {
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->println(F("Not connected to the ECU"));
        }
        setError(EE_USER);
        return;
    }

    if (_debug_level >= DEBUG_LEVEL_DEFAULT)
    {
        _debug->println(F("Requesting Sensors Data"));
    }

#if defined(GSXR)

    sendRequest(request_sens, LEN(request_sens));
    listenResponse();

#elif defined(NINJA)

    for (uint8_t request = 0; request < LEN(request_sens); request++)
    {
        sendRequest(request_sens[request], LEN(request_sens[request]));
        listenResponse();
        //increase addres of _response
    }

#elif defined(CBR)

#elif defined(R)

#endif

    //GPS (Gear Position Sensor)
    _GEAR1 = _response[PID_GPS];
    _GEAR2 = _response[PID_CLUTCH];
    _GEAR3 = _response[PID_GEAR_3];
    _GPS = 0;

    //RPM (Rights Per Minutes) it is split between two byte
    _RPM = _response[PID_RPM_H] * 10 + _response[PID_RPM_L] / 10;

    //Speed
    _SPEED = _response[PID_SPEED] * 2;

    //TPS (Throttle Position Sensor)
    _TPS = 125 * (_response[PID_TPS] - 55) / (256 - 55);

    //IAP (Intake Air Pressure)
    _IAP = _response[PID_IAP] * 4 * 0.136;

    //IAT (Intake Air Temperature)
    _IAT = (_response[PID_IAT] - 48) / 1.6;

    //ECT (Engine Coolant Temperature)
    _ECT = (_response[PID_ECT] - 48) / 1.6;

    //STPS (Secondary Throttle Position Sensor)
    _STPS = _response[PID_STPS] / 2.55;

    /*
    other sensors

    //voltage?
    voltage = _response[32] * 100 / 126;

    //FUEL 40-46

    //IGN 49-52

    //STVA
    STVA = _response[54] * 100 / 255;

    //pair
    PAIR = _response[59];
    */

// convert the temperature value if we are using them
#ifdef FAHRENHEIT
    _IAT = TO_FAHRENHEIT(_IAT);
    _ECT = TO_FAHRENHEIT(_ECT);
#endif

    _last_sensors_calculated = millis();
}

void KWP2000::keepAlive(uint16_t time)
{
    if (_kline->available() > 0)
    {
        // the ECU wants to tell something
        uint8_t in;
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->println("Me:\nHave you said something?\nECU:");
        }
        while (_kline->available() > 0)
        {
            in = _kline->read();
            if (_debug_level == DEBUG_LEVEL_VERBOSE)
            {
                _debug->println(in, HEX);
            }
        }
    }

    if (_ECU_status == false)
    {
        return; //if it is not connected it is meaningless to send a request
    }

    if (millis() - _last_correct_response >= ISO_T_P3_MAX)
    {
        // the connection has been lost
        connectionExpired();
        return;
    }

    if (time == 0)
    {
        time = _keep_iso_alive;
    }

    if (time > ISO_T_P3_MAX)
    {
        // prevent human's errors
        time = _keep_iso_alive;
        setError(EE_USER);
    }

    if (millis() - _last_correct_response <= time)
    {
        // not enough time has passed since last time we talked with the ECU
        return;
    }

    if (_debug_level == DEBUG_LEVEL_VERBOSE)
    {
        _debug->print(F("\nKeeping connection alive\nLast:"));
        _debug->println(millis() - _last_correct_response);
    }
#if defined(GSXR)
    sendRequest(request_sens, LEN(request_sens));
#elif defined(NINJA)
    sendRequest(request_sens[0], LEN(request_sens[0][0]));
#elif defined(CBR)
    sendRequest(request_sens, LEN(request_sens));
#elif defined(R)
    sendRequest(request_sens, LEN(request_sens));
#endif
    listenResponse();
}

////////////// COMMUNICATION - Advanced ////////////////
void KWP2000::accessTimingParameter(const uint8_t read_only)
{
    uint8_t p2_min_temp = _response[_response_data_start + 2];
    uint16_t p3_min_temp = _response[_response_data_start + 4];
    uint16_t p4_min_temp = _response[_response_data_start + 6];

    uint32_t p2_max_temp = _response[_response_data_start + 3];
    if (p2_max_temp <= 0xF0)
    {
        p2_max_temp *= 25;
    }
    else if ((p2_max_temp > 0xF0) && (p2_max_temp < 0xFF))
    {
        p2_max_temp = (p2_max_temp & 0xF) * 256 * 25;
    }
    else if (p2_max_temp == 0xFF)
    {
        p2_max_temp = 89601;
        setError(EE_ATP);
    }

    uint32_t p3_max_temp = _response[_response_data_start + 5];
    if (p3_max_temp <= 0xF0)
    {
        p3_max_temp *= 25;
    }
    else if ((p3_max_temp > 0xF0) && (p3_max_temp < 0xFF))
    {
        p3_max_temp = (p3_max_temp & 0xF) * 256 * 25;
    }
    else if (p3_max_temp == 0xFF)
    {
        p3_max_temp = 89601;
        setError(EE_ATP);
    }

    // any of these condition is an error
    if ((p2_min_temp > p2_max_temp) || (p3_min_temp > p3_max_temp) || (p4_min_temp > ISO_T_P4_MAX_LIMIT) || (p3_min_temp < p4_min_temp))
    {
        setError(EE_ATP);
    }

    if (read_only == false)
    {
        ISO_T_P2_MIN = p2_min_temp;
        ISO_T_P2_MAX = p2_max_temp;
        ISO_T_P3_MIN = p3_min_temp;
        ISO_T_P3_MAX = p3_max_temp;
        ISO_T_P4_MIN = p4_min_temp;

        // we set a safe margin to ask data enough often to the ECU
        _keep_iso_alive = p3_max_temp / 4;
    }

    if (_debug_level == DEBUG_LEVEL_VERBOSE)
    {
        _debug->println(F("Timing Parameter from the ECU:"));
        _debug->print("Errors:\t");
        _debug->println(bitRead(_ECU_error, EE_ATP) == 1 ? "Yes" : "No");
        _debug->print(F("P2 min:\t"));
        _debug->println(p2_min_temp);
        _debug->print(F("P2 max:\t"));
        _debug->println(p2_max_temp);
        _debug->print(F("P3 min:\t"));
        _debug->println(p3_min_temp);
        _debug->print(F("P3 max:\t"));
        _debug->println(p3_max_temp);
        _debug->print(F("P4 min:\t"));
        _debug->println(p4_min_temp);
        _debug->println();
    }
}

void KWP2000::changeTimingParameter(uint32_t new_atp[], const uint8_t new_atp_len)
{
    if ((new_atp == nullptr) || (new_atp_len == 0))
    {
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->println(F("Changing timing parameter not possible: wrong array or wrong lenght"));
        }
        setError(EE_USER);
        return;
    }

    if (new_atp_len != 5)
    {
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->println(F("The time paramenter should be an array of 5 elements"));
        }
        setError(EE_USER);
        return;
    }

    if (new_atp[0] > ISO_T_P2_MIN_LIMIT)
    {
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->println(F("P2 min too hight"));
        }
        return;
    }

    if (new_atp[1] > ISO_T_P2_MAX_LIMIT)
    {
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->println(F("P2 max too hight"));
        }
        return;
    }

    if (new_atp[2] > 255)
    {
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->println(F("P3 min too hight"));
        }
        return;
    }

    if (new_atp[3] > ISO_T_P3_MAX_LIMIT)
    {
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->println(F("P3 max too hight"));
        }
        return;
    }

    if (new_atp[4] > ISO_T_P4_MAX_LIMIT)
    {
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->println(F("P4 min too hight"));
        }
        return;
    }
    // all check passed

    // convert the data if P2 and P3 max are too high for a tiny byte
    uint32_t p2_max_temp = new_atp[1];
    uint32_t p3_max_temp = new_atp[3];

    if (p2_max_temp <= 6000)
    {
        p2_max_temp = p2_max_temp / 25;
    }
    else if (p2_max_temp > 6000)
    {
        p2_max_temp = (p2_max_temp / 256 / 25) | 0xf0;
    }
    new_atp[1] = p2_max_temp;

    if (p3_max_temp <= 6000)
    {
        p3_max_temp = p3_max_temp / 25;
    }
    else if (p3_max_temp > 6000)
    {
        p3_max_temp = (p3_max_temp / 256 / 25) | 0xf0;
    }
    new_atp[3] = p3_max_temp;

    // we need to create a new array made from the atp_set_given + new_atp + checksum
    uint8_t pid_temp[7];
    for (uint8_t n = 0; n < 7; n++)
    {
        if (n < 2) // atp_set_given
        {
            pid_temp[n] = atp_set_given[n];
        }
        else // new_atp
        {
            pid_temp[n] = new_atp[n - 2];
        }
    }

    // send it
    sendRequest(pid_temp, LEN(pid_temp));
    listenResponse();

    // check if our values has been setted correctly
    accessTimingParameter(true);
}

/////////////////// PRINT and GET ///////////////////////

void KWP2000::printStatus(uint16_t time)
{
    if (time == false)
    {
        // skip
        return;
    }

    if (millis() - _last_status_print <= time)
    {
        // not enough time has passed since last time we printed the status
        return;
    }

    if (_debug_enabled == true)
    {
        _debug->print(F("\n---- STATUS ----\n"));
        _debug->print(F("Connection:\t\t"));
        _debug->println(_ECU_status == 1 ? "Connected" : "Not connected");
        _debug->print(F("Errors:\t\t\t"));
        _debug->println(_ECU_error == 0 ? "No" : "Yes");
        _debug->print(F("Last data:\t\t"));
        _debug->println(_last_correct_response == 0 ? "Never"
                                                    : String((millis() - _last_correct_response) / 1000.0, 2) + " seconds ago");
        _debug->print(F("Connection time:"));
        _debug->println(_connection_time == 0 ? "Not now"
                                              : String((millis() - _connection_time) / 1000.0, 2) + " seconds ago");
        _debug->print(F("Baudrate:\t\t"));
        _debug->println(_kline_baudrate);
        _debug->print(F("K-line TX pin:\t"));
        _debug->println(_k_out_pin);
        _debug->print(F("Dealer pin:\t\t"));
        _debug->println(_dealer_pin);
        _debug->print(F("Dealer mode:\t"));
        _debug->println(_dealer_mode == 1 ? "Enabled" : "Disabled");

        //other stuff?
        if (_ECU_error != 0)
        {
            _debug->print(F("\n---- ERRORS ----\n"));
            for (uint8_t i = 0; i < EE_TOTAL; i++)
            {
                if (bitRead(_ECU_error, i) != 0)
                {
                    switch (i)
                    {
                    case EE_USER:
                        _debug->println(F("We called some function in a wrong way"));
                        break;
                    case EE_START:
                        _debug->println(F("Unable to start comunication"));
                        break;
                    case EE_STOP:
                        _debug->println(F("Unable to stop comunication"));
                        break;
                    case EE_TO:
                        _debug->println(F("Data is not for us"));
                        break;
                    case EE_FROM:
                        _debug->println(F("Data don't came from the ECU"));
                        break;
                    case EE_CS:
                        _debug->println(F("Checksum error"));
                        break;
                    case EE_ECHO:
                        _debug->println(F("Echo error"));
                        break;
                    case EE_MEMORY:
                        _debug->println(F("Memory error"));
                        break;
                    case EE_UNEX:
                        _debug->println(F("Unexpected error"));
                        break;
                    case EE_HEADER:
                        _debug->println(F("Unexpected header"));
                        break;
                    case EE_CONFIG:
                        _debug->println(F("The key bytes are probably wrong"));
                        break;
                    case EE_P3MAX:
                        _debug->println(F("Time out of the communication"));
                        break;
                    case EE_CR:
                        _debug->println(F("Check response error"));
                        break;
                    case EE_ATP:
                        _debug->println(F("Problem setting the timing parameter"));
                        break;
                    case EE_WR:
                        _debug->println(F("We get a reject for a request we didn't sent"));
                        break;
                    case EE_US:
                        _debug->println(F("Unsupported, yet"));
                        break;
                    default:
                        _debug->print(F("Did I forget any enum?"));
                        _debug->println(i);
                        break;
                    }
                }
            }
        }
        _debug->print(F("---- ------- ----\n\n"));
        _last_status_print = millis();
    }
    else
    {
        setError(EE_USER);
    }
}

void KWP2000::printSensorsData(uint16_t time)
{
    if (time == false)
    {
        // skip
        return;
    }

    if (millis() - _last_data_print <= time)
    {
        // not enough time has passed since last time we printed the sensor data
        return;
    }

    if (_last_sensors_calculated == 0)
    {
        // we didn't run requestSensorsData
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->print(F("requestSensorsData need to be called before"));
        }
        setError(EE_USER);
        return;
    }

    if (_debug_enabled == true)
    {
        _debug->print(F("---- SENSORS ----\n"));
        _debug->print(F("Calculated: "));
        _debug->print(_last_sensors_calculated == 0 ? "Never\n"
                                                    : String((millis() - _last_sensors_calculated) / 1000.0, 2) + " seconds ago\n");
        _debug->print(F("GPS:\t"));
        _debug->println(_GPS);
        _debug->print(F("RPM:\t"));
        _debug->println(_RPM);
        _debug->print(F("Speed:\t"));
        _debug->println(_SPEED);
        _debug->print(F("TPS:\t"));
        _debug->println(_TPS);
        _debug->print(F("IAP:\t"));
        _debug->println(_IAP);
        _debug->print(F("IAT:\t"));
        _debug->println(_IAT);
        _debug->print(F("ECT:\t"));
        _debug->println(_ECT);
        _debug->print(F("STPS:\t"));
        _debug->println(_STPS);
        //_debug->print(F(":\t"));_debug->println();

        _debug->print(F("_GEAR1:\t"));
        _debug->println(_GEAR1, BIN);
        _debug->print(F("_GEAR2:\t"));
        _debug->println(_GEAR2, BIN);
        _debug->print(F("_GEAR3:\t"));
        _debug->println(_GEAR3, BIN);

        _debug->print(F("---- ------- ----\n"));
        _last_data_print = millis();
    }
    else
    {
        setError(EE_USER);
    }
}

void KWP2000::printLastResponse()
{
    if (_debug_enabled == true)
    {
        _debug->println(F("Last Response from the ECU:"));
        for (uint8_t n = 0; n < _response_len; n++)
        {
            _debug->println(_response[n], HEX);
        }
    }
    else
    {
        setError(EE_USER);
    }
}

int8_t KWP2000::getStatus()
{
    return _ECU_status;
}

int8_t KWP2000::getError()
{
    if (_ECU_error == 0)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

void KWP2000::resetError()
{
    _ECU_error = 0;
}

uint8_t KWP2000::getGPS()
{
    return _GPS;
}

uint8_t KWP2000::getRPM()
{
    return _RPM;
}

uint8_t KWP2000::getSPEED()
{
    return _SPEED;
}

uint8_t KWP2000::getTPS()
{
    return _TPS;
}

uint8_t KWP2000::getIAP()
{
    return _IAP;
}

uint8_t KWP2000::getIAT()
{
    return _IAT;
}

uint8_t KWP2000::getECT()
{
    return _ECT;
}

uint8_t KWP2000::getSTPS()
{
    return _STPS;
}

/////////////////// PRIVATE ///////////////////////

void KWP2000::sendRequest(const uint8_t pid[], const uint8_t pid_len, const uint8_t wait_to_send_all, const uint8_t use_delay)
{
    uint8_t echo = 0;
    uint8_t header_len = 1; // minimun lenght

    // create the request
    // make the header
    if (_use_lenght_byte == true)
    {
        //we use the lenth byte
        _request[0] = format_physical;
        _request[3] = pid_len;
        header_len += 1;
    }
    else // don't use the lenght_byte
    {
        if (pid_len >= 64)
        {
            // we are forcet to use the lenght byte
            _request[0] = format_physical;
            _request[3] = pid_len;
            header_len += 1;
        }
        else
        {
            // the lenght byte is "inside" the format
            _request[0] = format_physical | pid_len;
        }
    }

    if (_use_target_source_address == true)
    {
        // add target and source address
        _request[1] = ECU_addr;
        _request[2] = OUR_addr;
        header_len += 2;
    }

    _request_len = header_len + pid_len + 1; // header + request + checksum

    // add the PID
    for (uint8_t k = 0; k < pid_len; k++)
    {
        _request[header_len + k] = pid[k];
    }

    // checksum
    _request[_request_len - 1] = calc_checksum(_request, _request_len - 1);

    // finally we send the request
    _elapsed_time = 0;
    for (uint8_t i = 0; i < _request_len; i++)
    {
        _kline->write(_request[i]);
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            if (i == 0)
            {
                _debug->println(F("\nSending\t\tEcho"));
            }
            _debug->println(_request[i], HEX);
        }

        _start_time = millis();
        while (_elapsed_time < ISO_T_P4_MIN)
        {
            _elapsed_time = millis() - _start_time;
            if (_kline->available() > 0)
            {
                echo = _kline->read();
                _debug->print("\t\t\t");
                _debug->println(echo, HEX);
            }
        }
        _elapsed_time = 0;

        //check if i send the correct bytes
        if (echo != _request[i] && echo != 0)
        {
            setError(EE_ECHO);
        }
    }
    _start_time = 0;

    if (wait_to_send_all == true)
    {
        _kline->flush();
    }

    if (use_delay == true)
    {
        delay(ISO_T_P2_MIN);
    }
}

void KWP2000::listenResponse(uint8_t *resp, uint8_t *resp_len, const uint8_t use_delay)
{
    // we passed only one of these two arguments
    if ((resp == nullptr) ^ (resp_len == nullptr))
    {
        setError(EE_USER);
        return;
    }

    uint8_t save = false;
    if (resp != nullptr && resp_len != nullptr)
    {
        save = true;
    }

    // reset _response
    _response_data_start = 0;
    _response_len = 0;
    for (uint16_t i = 0; i < ISO_MAX_DATA; i++)
    {
        _response[i] = 0;
        if (save)
        {
            resp[i] = 0;
        }
    }

    uint8_t masked = 0;                     // useful for bit mask operation
    uint8_t response_completed = false;     // when true no more bytes will be received
    uint32_t incoming;                      // incoming byte from the ECU
    uint8_t n_byte = 0;                     // actual lenght of the response, updated every times a new byte is received
    uint8_t data_to_rcv = 0;                // data to receive: bytes of the response that have to be received (not received yet)
    uint8_t data_rcvd = 0;                  // data received: bytes of the response already received
    uint32_t last_data_received = millis(); // check times for the timeout

    while ((millis() - last_data_received < ISO_T_P3_mdf) && (response_completed == false))
    {
        if (_kline->available() > 0)
        {
            incoming = _kline->read();
            _response[n_byte] = incoming;
            if (save)
            {
                resp[n_byte] = incoming;
            }

            if (_debug_level == DEBUG_LEVEL_VERBOSE)
            {
                if (n_byte == 0)
                {
                    _debug->print(F("\nReceiving:"));
                }
                _debug->print(F("\n"));
                _debug->print(incoming, HEX);
            }

            last_data_received = millis(); // reset the timer for each byte received

            // delay(ISO_T_P1);
            // Technically the ECU waits between 0 to 20 ms between sending two bytes
            // We use this time to analyze what we received

            switch (n_byte)
            {
            case 0: // the first byte is the formatter, with or without lenght bits

                masked = incoming & 0xC0; // 0b11000000
                if (masked == format_physical)
                {
                    if (_debug_level == DEBUG_LEVEL_VERBOSE)
                    {
                        _debug->print(F("\t- format physical"));
                    }
                }
                else if (masked == format_functional)
                {
                    if (_debug_level == DEBUG_LEVEL_VERBOSE)
                    {
                        _debug->print(F("\t- format functional"));
                    }
                    setError(EE_US);
                }
                else if (masked == format_CARB)
                {
                    if (_debug_level == DEBUG_LEVEL_VERBOSE)
                    {
                        _debug->print(F("\t- format CARB"));
                    }
                    setError(EE_US);
                }
                else
                {
                    if (_debug_level == DEBUG_LEVEL_VERBOSE)
                    {
                        _debug->print(F("\t- unexpected header"));
                    }
                    setError(EE_HEADER);
                }

                // let's see if there are lenght bits
                if (_use_lenght_byte == true || _use_lenght_byte == maybe)
                {
                    masked = incoming & 0x3F; // 0b00111111
                    if (masked != 0)          // the response lengh is inside the formatter
                    {
                        data_to_rcv = masked;
                        if (_debug_level == DEBUG_LEVEL_VERBOSE)
                        {
                            _debug->print(F("\t- "));
                            _debug->print(data_to_rcv);
                            _debug->print(F(" data bytes coming"));
                        }

                        if (_use_lenght_byte == maybe)
                        {
                            _use_lenght_byte = false;
                            setError(EE_TEST);
                        }
                    }
                    else // the response lenght is in a separete byte (the 2nd or the 4th)
                    {
                        data_to_rcv = 0;
                    }
                }
                break;

            case 1: // the second byte is be the target address or the lenght byte or the data

                if (_use_target_source_address == maybe)
                {
                    if (incoming == OUR_addr)
                    {
                        _use_target_source_address = true;
                        setError(EE_TEST);
                    }
                    else
                    {
                        _use_target_source_address = false;
                        setError(EE_TEST);
                    }
                }

                if (_use_target_source_address == true)
                {
                    if (incoming == OUR_addr) // it is the target byte
                    {
                        if (_debug_level == DEBUG_LEVEL_VERBOSE)
                        {
                            _debug->print(F("\t- ECU is communicating with us"));
                        }
                    }
                    else
                    {
                        if (_debug_level == DEBUG_LEVEL_VERBOSE)
                        {
                            // (I'm not jealous it's just curiosity)
                            _debug->print(F("\t- ECU is communicating with this address"));
                        }
                        setError(EE_TO);
                    }
                }
                else if (_use_target_source_address == false)
                {
                    if (data_to_rcv == 0) // it is the lenght byte
                    {
                        data_to_rcv = incoming;
                        if (_debug_level == DEBUG_LEVEL_VERBOSE)
                        {
                            _debug->print(F("\t- "));
                            _debug->print(data_to_rcv);
                            _debug->print(F(" data bytes coming"));
                        }
                    }
                    else // data
                    {
                        data_rcvd++;
                        if (_debug_level == DEBUG_LEVEL_VERBOSE)
                        {
                            _debug->print(F("\t- data"));
                        }
                        if (_response_data_start == 0)
                        {
                            _response_data_start = n_byte;
                        }
                    }
                }
                break;

            case 2: // the third byte is the source address or the data or checksum

                if (_use_target_source_address == true)
                {
                    if (incoming == ECU_addr)
                    {
                        if (_debug_level == DEBUG_LEVEL_VERBOSE)
                        {
                            _debug->print(F("\t- comes from the ECU"));
                        }
                    }
                    else
                    {
                        if (_debug_level == DEBUG_LEVEL_VERBOSE)
                        {
                            // check who sent it
                            _debug->print(F("\t- doesn't come from the ECU"));
                        }
                        setError(EE_FROM);
                    }
                }
                else // data or checksum
                {
                    if (data_to_rcv == data_rcvd) // it is the checksum
                    {
                        response_completed = true;
                        _response_len = n_byte;
                        if (save)
                        {
                            *resp_len = n_byte;
                        }
                        endResponse(incoming);
                    }
                    else // there is still data outside
                    {
                        data_rcvd++;
                        if (_debug_level == DEBUG_LEVEL_VERBOSE)
                        {
                            _debug->print(F("\t- data"));
                        }
                        if (_response_data_start == 0)
                        {
                            _response_data_start = n_byte;
                        }
                    }
                }
                break;

            case 3: // the fourth byte is the lenght byte or the data or checksum

                if (data_to_rcv == 0) // it is the lenght byte
                {
                    data_to_rcv = incoming;
                    if (_debug_level == DEBUG_LEVEL_VERBOSE)
                    {
                        _debug->print(F("\t- data bytes coming in HEX"));
                    }
                }
                else // data or checksum
                {
                    if (data_to_rcv == data_rcvd) // it is the checksum
                    {
                        response_completed = true;
                        _response_len = n_byte;
                        if (save)
                        {
                            *resp_len = n_byte;
                        }
                        endResponse(incoming);
                    }
                    else // data
                    {
                        data_rcvd++;
                        if (_debug_level == DEBUG_LEVEL_VERBOSE)
                        {
                            _debug->print(F("\t- data"));
                        }
                        if (_response_data_start == 0)
                        {
                            _response_data_start = n_byte;
                        }
                    }
                }
                break;

            default: // data or checksum

                if (data_to_rcv == data_rcvd) // it is the checksum
                {
                    response_completed = true;
                    _response_len = n_byte;
                    if (save)
                    {
                        *resp_len = n_byte;
                    }
                    endResponse(incoming);
                }
                else // data
                {
                    data_rcvd++;
                    if (_debug_level == DEBUG_LEVEL_VERBOSE)
                    {
                        _debug->print(F("\t- data"));
                    }
                    if (_response_data_start == 0)
                    {
                        _response_data_start = n_byte;
                    }
                }
                break;
            }         // end of the swith statement
            n_byte++; // read the next byte of the response
        }             // end of the if _kline.available()
    }                 // end of the while timeout

    if (use_delay == true)
    {
        delay(ISO_T_P3_MIN);
    }
}

int8_t KWP2000::checkResponse(const uint8_t response_sent[])
{
    if (_response[_response_data_start] == (request_ok(response_sent[0])))
    {
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->println(F("\nCorrect response from the ECU\n"));
        }
        return 1;
    }
    else if (_response[_response_data_start] == 0)
    {
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->println(F("\nNo response from the ECU\n"));
        }
        return -1;
    }
    else if (_response[_response_data_start] == request_rejected)
    {
        setError(EE_CR);
        if (_response[_response_data_start + 1] != response_sent[0])
        {
            setError(EE_WR);
        }

        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->print(F("\nRequest rejected by the ECU with code: "));
        }

        if (_response[_response_data_start + 2] == 0x10)
        {
            if (_debug_level == DEBUG_LEVEL_VERBOSE)
            {
                _debug->println(F("General\n"));
            }
            return -2;
        }
        else if (_response[_response_data_start + 2] == 0x11) //todo
        {
            if (_debug_level == DEBUG_LEVEL_VERBOSE)
            {
                _debug->println(F("todo\n"));
            }
            return -3;
        } //and so on
        else
        {
            if (_debug_level == DEBUG_LEVEL_VERBOSE)
            {
                _debug->println(F("Unknown error code\n"));
            }
            return -6;
        }
    }
    else
    {
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->print(F("\nUnexpected response: "));
            for (uint8_t n = _response_data_start; n < _response_len; n++)
            {
                _debug->println(_response[n], HEX);
            }
        }
        setError(EE_CR);
        setError(EE_UNEX);
        return -8;
    }
    // the program should never arrive here
    setError(EE_UNEX);
    return -9;
}

void KWP2000::setError(const uint8_t error)
{
    bitSet(_ECU_error, error);
}

void KWP2000::configureKline()
{
    // get the key bytes
    if (_response[_response_data_start + 2] != 0x8F)
    {
        setError(EE_CONFIG);
    }

    uint16_t key_bytes = _response[_response_data_start + 2] << 8 | _response[_response_data_start + 1];

    // lenght byte
    uint8_t AL0 = bitRead(key_bytes, 0);
    uint8_t AL1 = bitRead(key_bytes, 1);
    if (AL1 == 1 && AL0 == 1)
    {
        // both are possible, so choose the faster one
        _use_lenght_byte = false;
    }
    else if (AL1 == 1 && AL0 == 0)
    {
        // lenght byte must be present
        _use_lenght_byte = true;
    }
    else if (AL1 == 0 && AL0 == 1)
    {
        // lenght byte not needed
        _use_lenght_byte = false;
    }

    // target and source
    uint8_t HB0 = bitRead(key_bytes, 2);
    uint8_t HB1 = bitRead(key_bytes, 3);
    if (HB1 == 1 && HB0 == 1)
    {
        // both are possible, so choose the faster one
        _use_target_source_address = false;
    }
    else if (HB1 == 1 && HB0 == 0)
    {
        // target and source address bytes must be present
        _use_target_source_address = true;
    }
    else if (HB1 == 0 && HB0 == 1)
    {
        // target and source address bytes not needed
        _use_target_source_address = false;
    }

    // timing
    uint8_t TP0 = bitRead(key_bytes, 4);
    uint8_t TP1 = bitRead(key_bytes, 5);
    if (TP1 == 1 && TP0 == 1)
    {
        setError(EE_CONFIG);
    }
    else if (TP1 == 1 && TP0 == 0)
    {
        _timing_parameter = true; // normal
    }
    else if (TP1 == 0 && TP0 == 1)
    {
        _timing_parameter = false; // extended
        // this allow faster comunication but I don't plan to implement it soon
        setError(EE_US);
    }

    if (AL0 == 0 && AL1 == 0 && HB0 == 0 && HB1 == 0 && TP0 == 1 && TP1 == 0)
    {
        // it will use the same configuration as in the first response
        _use_lenght_byte = maybe;
        _use_target_source_address = maybe;
        _timing_parameter = maybe;
    }

    // must be 1
    if (bitRead(key_bytes, 6) != 1)
    {
        setError(EE_CONFIG);
    }

    // parity of the 6 bits above
    uint8_t parity = 1;
    for (uint8_t n = 0; n < 7; n++)
    {
        if (bitRead(key_bytes, n))
        {
            parity = !parity;
        }
    }

    if (bitRead(key_bytes, 7) != parity)
    {
        setError(EE_CONFIG);
    }

    if (_debug_level == DEBUG_LEVEL_VERBOSE)
    {
        _debug->println("\nK line config:");
        _debug->print("Key bytes:\t\t\t0x");
        _debug->print(key_bytes, HEX);
        _debug->print(" - ");
        _debug->println(key_bytes, BIN);
        _debug->print("Errors:\t\t\t\t");
        _debug->println(bitRead(_ECU_error, EE_CONFIG) == 1 ? "Yes" : "No");
        _debug->print("Lenght byte:\t\t");
        _debug->println(_use_lenght_byte == 1 ? "Yes" : "No");
        _debug->print("Addresses bytes:\t");
        _debug->println(_use_target_source_address == 1 ? "Yes" : "No");
        _debug->print("Timing parameter:\t");
        _debug->println(_timing_parameter == 1 ? "Normal\n" : "Extended\n");
    }
}

// Checksum is the sum of all data bytes modulo (&) 0xFF
// (same as being truncated to one byte)
uint8_t KWP2000::calc_checksum(const uint8_t data[], const uint8_t data_len)
{
    uint8_t cs = 0;
    for (uint8_t i = 0; i < data_len; i++)
    {
        cs += data[i];
    }
    return cs;
}

void KWP2000::endResponse(const uint8_t received_checksum)
{
    uint8_t correct_checksum;

    if (_debug_level == DEBUG_LEVEL_VERBOSE)
    {
        _debug->println(F("\t- checksum"));
        _debug->println(F("\nEnd of response"));
        _debug->print(F("Bytes received: "));
        _debug->println(_response_len);
    }

    correct_checksum = calc_checksum(_response, _response_len);
    if (correct_checksum == received_checksum)
    {
        // the checksum is correct and everything went well!
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->println(F("Correct checksum"));
        }
        _last_correct_response = millis();
    }
    else // the checksum is not correct
    {
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->print(F("Wrong checksum, expected: "));
            _debug->println(correct_checksum, HEX);
        }
        setError(EE_CS);
    }
}

void KWP2000::connectionExpired()
{
    if (_stop_sequence_started == false)
    {
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->println(F("\nConnection expired"));
        }
        _ECU_status = false;
        _last_data_print = 0;
        _last_sensors_calculated = 0;
        _last_status_print = 0;
        _connection_time = 0;
        _kline->end();
        setError(EE_P3MAX);
    }
}
