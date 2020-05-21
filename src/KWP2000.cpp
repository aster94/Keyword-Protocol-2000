/*
KWP2000.cpp

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

#ifndef KWP2000_cpp
#define KWP2000_cpp

#include <Arduino.h>

#include "KWP2000.h"
#include "ISO.h"

#warning "Not ready for Yamaha and Honda, before using this code open an issue on github"

#define maybe 2 ///< used when we don't know yet the behaviour of the K-Line

#define TO_FAHRENHEIT(x) x * 1.8 + 32                                                   ///< the formula for the conversion from celsius to fahrenheit
#define TO_MPH(x) x / 1.609                                                             ///< the formula for the conversion from km/h to mp/h
#define LEN(x) ((sizeof(x) / sizeof(0 [x])) / ((size_t)(!(sizeof(x) % sizeof(0 [x]))))) ///< complex but safe macro for the length

/**
 * @brief This is a a collection of possible ECU Errors
 */
enum error_enum
{
    EE_TEST,   ///< for test purposes
    EE_START,  ///< unable to start comunication
    EE_STOP,   ///< unable to stop comunication
    EE_TO,     ///< data is not for us
    EE_FROM,   ///< data don't came from the ECU
    EE_CS,     ///< checksum error
    EE_ECHO,   ///< echo error
    EE_UNEX,   ///< unexpected error
    EE_HEADER, ///< header not regular
    EE_USER,   ///< error due to wrong call of a function
    EE_CONFIG, ///< key bytes not regular
    EE_P3MAX,  ///< time out communication
    EE_CR,     ///< check response error
    EE_ATP,    ///< problem setting/reading the timing parameter
    EE_WR,     ///< We get a reject for a request we didn't sent
    EE_US,     ///< not supported, yet
    EE_TOTAL   ///< this is just to know how many possible errors are in this enum
};

////////////// CONSTRUCTOR ////////////////

/**
 * @brief Constructor for the KWP2000 class
 *
 * @param kline_serial The Serial port you will use to communicate with the ECU
 * @param k_out_pin The TX pin of this serial
 * @param brand the brand of your bike: `SUZUKI`, `KAWASAKI`, `YAMAHA` or `HONDA`
 * @param model The model of the bike
 */
KWP2000::KWP2000(HardwareSerial *kline_serial, const uint8_t k_out_pin, const brand brand, const model model)
{
    _kline = kline_serial;
    _k_out_pin = k_out_pin;
    _brand = brand;
    _model = model;
    set_bike_specific_values(_brand, _model);

    _response = new uint8_t[ISO_MAX_DATA];
}

////////////// SETUP ////////////////

/**
 * @brief Enable the debug of the communication
 *
 * @param debug_serial The Serial port you will use for the debug information
 * @param debug_level The verbosity of the debug, default to `DEBUG_LEVEL_DEFAULT`
 * @param debug_baudrate The baudrate for the debug, default to `115200`
 */
void KWP2000::enableDebug(HardwareSerial *debug_serial, const uint8_t debug_level, const uint32_t debug_baudrate)
{
    _debug = debug_serial;
    _debug_level = debug_level;
    _debug->begin(debug_baudrate);
    _debug_enabled = true;

    if (_debug_level >= DEBUG_LEVEL_DEFAULT)
    {
        _debug->println(F("Debug enabled"));
    }
}

/**
 * @brief Change the debug level
 *
 * @param debug_level choose between DEBUG_LEVEL_NONE DEBUG_LEVEL_DEFAULT DEBUG_LEVEL_VERBOSE
 */
void KWP2000::setDebugLevel(const uint8_t debug_level)
{
    _debug_level = debug_level;
    if (_debug_level == DEBUG_LEVEL_NONE)
    {
        _debug_enabled = false;
    }
    else
    {
        _debug_enabled = true;
    }
    if (_debug_level >= DEBUG_LEVEL_DEFAULT)
    {
        _debug->print(F("Debug level: "));
        _debug->println(debug_level == DEBUG_LEVEL_DEFAULT ? "default" : "verbose");
    }
}

/**
 * @brief Disable the debug
 */
void KWP2000::disableDebug()
{
    if (_debug_level >= DEBUG_LEVEL_DEFAULT)
    {
        _debug->println(F("Debug disabled"));
    }
    _debug->end();
    _debug_enabled = false;
}

/**
 * @brief Only for Suzuki: Enable the Dealer Mode
 *
 * @param dealer_pin The pin you will use to control it
 */
void KWP2000::enableDealerMode(const uint8_t dealer_pin)
{
    if (_brand != SUZUKI)
    {
        return;
    }
    _dealer_enabled = true;
    _dealer_pin = dealer_pin;
    pinMode(_dealer_pin, OUTPUT);
    digitalWrite(_dealer_pin, LOW);
}

/**
 * @brief Only for Suzuki: Enable/Disable the Dealer Mode
 *
 * @param dealer_mode Choose between true/false
 */
void KWP2000::setDealerMode(const uint8_t dealer_mode)
{
    if (_dealer_enabled == false)
    {
        return;
    }
    _dealer_mode = dealer_mode;
    digitalWrite(_dealer_pin, _dealer_mode);
    if (_debug_level >= DEBUG_LEVEL_DEFAULT)
    {
        _debug->print(F("Dealer mode: "));
        _debug->println(_dealer_mode == true ? "Enabled" : "Disabled");
    }
}

/**
 * @brief Only for Suzuki: Check the dealer status
 *
 * @return true if it is in dealer mode
 * @return false otherwise
 */
bool KWP2000::getDealerMode()
{
    if (_dealer_enabled == false)
    {
        return false;
    }
    return digitalRead(_dealer_pin);
}

/**
 * @brief Choose to use imperial system for the sensors values (mp/h, Fahrenheit)
 */
void KWP2000::use_imperial()
{
    _use_metric_system = false;
}

/**
 * @brief Choose to use metric system for the sensors values (km/h, Celsius)
 */
void KWP2000::use_metric()
{
    _use_metric_system = true;
}

////////////// COMMUNICATION - Basic ////////////////

/**
 * @brief Initialize the the communication through the K-Line
 *
 * @return `false` until the connection is not established; `true` if we connected; a `negative number` otherwise
 */
int8_t KWP2000::initKline()
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

        /*
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
        */

        _use_length_byte = false;
        if (_brand == SUZUKI || _brand == KAWASAKI)
        {
            _use_target_source_address = true;
        }
        else if (_brand == YAMAHA || _brand == HONDA)
        {
            _use_target_source_address = false;
        }

        _kline->end();

        digitalWrite(_k_out_pin, HIGH);
        pinMode(_k_out_pin, OUTPUT);

        _start_time = millis();
        _elapsed_time = 0;
        _init_phase = 0;
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->println(F("Starting sequence"));
        }
    }
    _elapsed_time = millis() - _start_time;

    switch (_init_phase)
    {
    case 0: // this is a 1000ms HIGH signal
        if (_elapsed_time > ISO_T_IDLE)
        {
            _init_phase++;
        }
        break;
    case 1:
        digitalWrite(_k_out_pin, LOW);
        _init_phase++;
        break;
    case 2: // this is a 25ms LOW signal
        if (_elapsed_time > ISO_T_IDLE + ISO_T_INIL)
        {
            _init_phase++;
        }
        break;
    case 3:
        digitalWrite(_k_out_pin, HIGH);
        _init_phase++;
        break;
    case 4: // this is a 25ms HIGH signal
        if (_elapsed_time > ISO_T_IDLE + ISO_T_WUP)
        {
            _init_phase++;
        }
        break;
    case 5:
        _init_sequence_started = false;
        _start_time = 0;
        _elapsed_time = 0;
        _kline->begin(ISO_BAUDRATE);

        if (!handleRequest(start_com, LEN(start_com), true))
        {
            if (_debug_level >= DEBUG_LEVEL_DEFAULT)
            {
                _debug->println(F("Initialization failed"));
            }
            _ECU_status = false;
            //ISO_T_IDLE = 0;
            setError(EE_START);
            return -2;
        }

        configureKline(); // maybe honda e yamaha shouldn't run this

        if (_brand == KAWASAKI)
        {
            if (_debug_level >= DEBUG_LEVEL_DEFAULT)
            {
                _debug->println(F("First handshake ok, now starting diagnostic session"));
            }
            if (!handleRequest(start_diagnostic, LEN(start_diagnostic)))
            {
                if (_debug_level >= DEBUG_LEVEL_DEFAULT)
                {
                    _debug->println(F("Failed to start diagnostic"));
                }
                _ECU_status = false;
                //ISO_T_IDLE = 0;
                setError(EE_START);
                return -2;
            }
            if (_debug_level >= DEBUG_LEVEL_DEFAULT)
            {
                _debug->println(F("Start diagnostic successful"));
            }
        }

        if (_debug_level >= DEBUG_LEVEL_DEFAULT)
        {
            _debug->println(F("ECU connected"));
        }
        _connection_time = millis();
        _ECU_status = true;
        _ECU_error = 0;
        return 1;
        break;
    default:
        break;
    }

    return 0;
}

/**
 * @brief Close the communication with the motorbike
 *
 * @return `false` until the connection is not closed; `true` if there aren't any errors, a `negative number` otherwise
 */
int8_t KWP2000::stopKline()
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

        // reset all
        for (uint16_t i = 0; i < ISO_MAX_DATA; i++)
        {
            _response[i] = 0;
        }
        _response_len = 0;
        _response_data_start = 0;

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

/**
 * @brief Send a request to the ECU asking for data from all the sensors, then you can access the single sensor with the `get*()` functions or you can see all of them with the `printSensorsData()`
 */
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

    if (_brand == SUZUKI)
    {
        handleRequest(suzuki_request_sens, LEN(suzuki_request_sens));
        //GPS (Gear Position Sensor)
        _GPS = _response[IDX_GPS];

        // Clutch
        _CLUTCH = _response[IDX_CLUTCH];

        //RPM (Rights Per Minutes) it is split between two byte
        _RPM = _response[IDX_RPM_H] * 10 + _response[IDX_RPM_L] / 10;

        // Speed
        _SPEED = _response[IDX_SPEED] * 2;

        // TPS (Throttle Position Sensor)
        _TPS = 125 * (_response[IDX_TPS] - 55) / (256 - 55);

        // STPS (Secondary Throttle Position Sensor)
        _STPS = _response[IDX_STPS] / 2.55;

        // IAP (Intake Air Pressure)
        _IAP = _response[IDX_IAP] * 4 * 0.136;

        // IAT (Intake Air Temperature)
        _IAT = (_response[IDX_IAT] - 48) / 1.6;

        // ECT (Engine Coolant Temperature)
        _ECT = (_response[IDX_ECT] - 48) / 1.6;

        // Voltage
        _VOLT = _response[IDX_VOLT] * 100 / 126;
    }
    else if (_brand == KAWASAKI)
    {
        handleRequest(kawasaki_request_gps, LEN(kawasaki_request_gps));
        _GPS = _response[IDX_GPS];

        handleRequest(kawasaki_request_rpm, LEN(kawasaki_request_rpm));
        _RPM = ((_response[IDX_RPM_H] * 255 + _response[IDX_RPM_L]) / 255.0) * 100;

        handleRequest(kawasaki_request_speed, LEN(kawasaki_request_speed));
        _SPEED = _response[IDX_SPEED];

        handleRequest(kawasaki_request_tps, LEN(kawasaki_request_tps));
        _TPS = _response[IDX_TPS];

        handleRequest(kawasaki_request_iap, LEN(kawasaki_request_iap));
        _IAP = _response[IDX_IAP] * 4 * 0.136;

        handleRequest(kawasaki_request_iat, LEN(kawasaki_request_iat));
        _IAT = (_response[IDX_IAT] - 48) / 1.6;

        handleRequest(kawasaki_request_ect, LEN(kawasaki_request_ect));
        _ECT = (_response[IDX_ECT] - 48) / 1.6;
    }
    else if (_brand == YAMAHA)
    {
        handleRequest(yamaha_request_sens, LEN(yamaha_request_sens));
    }
    else if (_brand == HONDA)
    {
        handleRequest(honda_request_sens, LEN(honda_request_sens));
    }

    _last_sensors_calculated = millis();
}

/**
 * @brief Read the Diagnostic Trouble Codes (DTC) from the ECU
 *
 * @param Default to `READ_ONLY_ACTIVE`, accepted values `READ_TOTAL`, `READ_ALL`
 */
void KWP2000::readTroubleCodes(const trouble_code which)
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

    if (which == READ_TOTAL)
    {
        handleRequest(trouble_codes_all, LEN(trouble_codes_all));
    }
    else if (which == READ_ONLY_ACTIVE)
    {
        handleRequest(trouble_codes_only_active, LEN(trouble_codes_only_active));
    }
    else if (which == READ_ALL)
    {
        handleRequest(trouble_codes_with_status, LEN(trouble_codes_with_status));
    }

    const uint8_t DTC_total = _response[_response_data_start + 1]; // Diagnosis trouble codes
    if (_debug_level >= DEBUG_LEVEL_DEFAULT)
    {
        _debug->print("There are ");
        _debug->print(DTC_total);
        _debug->println(" errors\n");
        for (uint8_t n = _response_data_start + 2; n < _response_len; n++)
        {
            _debug->print(_response[n]); // todo needed more test to understand the DTC number, value and status parameters
        }
        _debug->println();
    }
}

/**
 * @brief Clear the DTC from the ECU
 *
 * @param code Optional. Only the passed `code` will be cleared
 */
void KWP2000::clearTroubleCodes(const uint8_t code)
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

    if (code == 0x00) // Clear all
    {
        handleRequest(clear_trouble_codes, LEN(clear_trouble_codes));
    }
    else // Clear a single error provided by the user
    {
        const uint8_t to_clear[] = {clear_trouble_codes[0], code};
        handleRequest(to_clear, LEN(to_clear));
    }
}

/**
 * @brief Keep the connection through the K-Line alive
 *
 * @param time Optional. It is calculated automatically to be a safe interval
 */
void KWP2000::keepAlive(uint16_t time)
{
    if (_kline->available() > 0)
    {
        // the ECU wants to tell something
        uint8_t in;
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->println("Serial buffer not empty:");
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
        return; // If it is not connected it is meaningless to send a request
    }

    if (millis() - _last_correct_response >= ISO_T_P3_MAX)
    {
        // The connection has been lost
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
        return;
    }

    if (time == 0)
    {
        time = _keep_iso_alive;
    }

    if (time > ISO_T_P3_MAX)
    {
        // Prevent human's errors
        time = _keep_iso_alive;
        setError(EE_USER);
    }

    if (millis() - _last_correct_response <= time)
    {
        // We talked with the ECU not so much time ago
        return;
    }

    if (_debug_level == DEBUG_LEVEL_VERBOSE)
    {
        _debug->print(F("\nKeeping connection alive\nLast:"));
        _debug->println(millis() - _last_correct_response);
    }

    // Send a dummy request
    if (_brand == SUZUKI)
    {
        handleRequest(tester_present_with_answer, LEN(tester_present_with_answer));
    }
    else if (_brand == KAWASAKI)
    {
        handleRequest(kawasaki_request_gps, LEN(kawasaki_request_gps));
    }
    else if (_brand == YAMAHA)
    {
        handleRequest(tester_present_with_answer, LEN(tester_present_with_answer));
    }
    else if (_brand == HONDA)
    {
        handleRequest(tester_present_with_answer, LEN(tester_present_with_answer));
    }
}

////////////// COMMUNICATION - Advanced ////////////////
/**
 * @brief This function is the core of the library. You just need to give a PID and it will generate the header, calculate the checksum and try to send the request.
 *          Then it will check if the response is correct and if now it will try to send the request another two times, all is based on the ISO14230
 *
 * @param to_send The PID you want to send, see PID.h for more detail
 * @param send_len The length of the PID (use `sizeof` to get it)
 * @param try_once Optional, default to `false`. Choose if you want to try to send the request 3 times in case of error
 * @return `true` if the request has been sent and a correct response has been received, `false` otherwise
 */
int8_t KWP2000::handleRequest(const uint8_t to_send[], const uint8_t send_len, const uint8_t try_once)
{
    uint8_t attempt;
    uint8_t completed = false;

    if (try_once == true)
    {
        attempt = 3;
    }
    else
    {
        attempt = 1;
    }

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
            if (_debug_level == DEBUG_LEVEL_VERBOSE && try_once == false)
            {
                _debug->print(F("Attempt "));
                _debug->print(attempt);
                _debug->print(F(" not luckily"));
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
        // We made more than 3 attemps so there is a problem
        return false;
    }
}

/**
 * @brief Read the time parameter and set some library timing based on them
 */
/*
void KWP2000::checkTimingParameter()
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
            _debug->println(F("Error reading timing limits"));
        }
        setError(EE_ATP);
    }

    if (_debug_level == DEBUG_LEVEL_VERBOSE)
    {
        _debug->print(F("Reading current timing paramenters"));
    }
    if (handleRequest(atp_read_current, LEN(atp_read_current)) == true)
    {
        accessTimingParameter(false);
    }
    else
    {
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->println(F("Error reading current timing paramenters"));
        }
        setError(EE_ATP);
    }
}
*/
/**
 * @brief Print the Timing Parameters from the ECU
 *
 * @param read_only Optional, default to `true`. This avoid the possibility to unintentionally change them
 */
/*
void KWP2000::accessTimingParameter(const uint8_t read_only)
{
    uint8_t p2_min_temp = _response[_response_data_start + 2];
    uint32_t p2_max_temp = _response[_response_data_start + 3];
    uint16_t p3_min_temp = _response[_response_data_start + 4];
    uint32_t p3_max_temp = _response[_response_data_start + 5];
    uint16_t p4_min_temp = _response[_response_data_start + 6];

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
*/
/**
 * @brief Reset the Timing Parameters to the default settings from the ECU
 */
/*
void KWP2000::resetTimingParameter()
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
        _debug->println(F("Resetting time parameters to default"));
    }
    if (handleRequest(atp_set_default, LEN(atp_set_default)) == true)
    {
        if (_debug_level >= DEBUG_LEVEL_DEFAULT)
        {
            _debug->println(F("Changed"));
        }
    }
    else
    {
        if (_debug_level >= DEBUG_LEVEL_DEFAULT)
        {
            _debug->println(F("Not changed"));
        }
    }
    accessTimingParameter(true);
}
*/
/**
 * @brief Change the Timing Parameters to custom ones
 *
 * @param new_atp Array of 5 elements containing the new parameters
 * @param new_atp_len The length of the array (use `sizeof` to get it)
 */
/*
void KWP2000::changeTimingParameter(uint32_t new_atp[], const uint8_t new_atp_len)
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
        _debug->println(F("Changing timing parameter"));
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
    if (handleRequest(pid_temp, LEN(pid_temp)) == true)
    {
        if (_debug_level >= DEBUG_LEVEL_DEFAULT)
        {
            _debug->println(F("Changed"));
        }
    }
    else
    {
        if (_debug_level >= DEBUG_LEVEL_DEFAULT)
        {
            _debug->println(F("Not changed"));
        }
    }

    // check if our values has been setted correctly
    accessTimingParameter(true);
}
*/
/////////////////// PRINT and GET ///////////////////////

/**
 * @brief Print a rich and useful set of information about the ECU status and errors
 *
 * @param time Optional, default to 5 seconds. The time between one print and the next one
 */
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

        if (_last_correct_response != 0)
        {
            _debug->print(F("Last data:\t\t"));
            _debug->print((millis() - _last_correct_response) / 1000.0);
            _debug->print(F(" seconds ago"));
        }

        if (_connection_time != 0)
        {
            _debug->print(F("Connection time:"));
            _debug->print((millis() - _connection_time) / 1000.0);
            _debug->println(" seconds ago");
        }

        _debug->print(F("Baudrate:\t\t"));
        _debug->println(ISO_BAUDRATE);
        _debug->print(F("K-line TX pin:\t\t"));
        _debug->println(_k_out_pin);
        if (_brand == SUZUKI)
        {
            _debug->print(F("Dealer pin:\t\t"));
            _debug->println(_dealer_pin);
            _debug->print(F("Dealer mode:\t"));
            _debug->println(_dealer_mode == 1 ? "Enabled" : "Disabled");
        }
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

/**
 * @brief Print all the sensors data from the ECU, you need to run `requestSensorsData()` before
 */
void KWP2000::printSensorsData()
{
    if (_last_sensors_calculated == 0)
    {
        // we didn't run requestSensorsData
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->print(F("requestSensorsData() needs to be called before"));
        }
        setError(EE_USER);
        return;
    }

    if (_debug_enabled == true)
    {
        _debug->print(F("---- SENSORS ----\n"));
        _debug->print(F("Calculated: "));
        _debug->print(millis() - _last_sensors_calculated);
        _debug->println(F(" milliseconds ago"));
        _debug->print(F("GPS:\t"));
        _debug->println(_GPS);
        _debug->print(F("CLUTCH:\t"));
        _debug->println(_CLUTCH);
        _debug->print(F("RPM:\t"));
        _debug->println(_RPM);
        _debug->print(F("Speed:\t"));
        _debug->println(_SPEED);
        _debug->print(F("TPS:\t"));
        _debug->println(_TPS);
        _debug->print(F("STPS:\t"));
        _debug->println(_STPS);
        _debug->print(F("IAP:\t"));
        _debug->println(_IAP);
        _debug->print(F("IAT:\t"));
        _debug->println(_IAT);
        _debug->print(F("ECT:\t"));
        _debug->println(_ECT);
        _debug->print(F("VOLT:\t"));
        _debug->println(_VOLT);

        _debug->print(F("---- ------- ----\n"));
        _last_data_print = millis();
    }
    else
    {
        setError(EE_USER);
    }
}

/**
 * @brief Print the last response received from the ECU
 */
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

/**
 * @brief Get the connection status
 *
 * @return It could be `true` or `false`
 */
int8_t KWP2000::getStatus()
{
    return _ECU_status;
}

/**
 * @brief Get* the ECU sensor value you need
 * GPS: Gear Position Sensor
 * CLUTCH: Know if the clutch is pressed or not
 * RPM: Revolutions per Minutes
 * SPEED: Speed
 * TPS: Throttle Position Sensor
 * STPS: Secondary Throttle Position Sensor
 * IAP: Intake Air Pressure, in kPa
 * IAT: Intake Air Temperature
 * ECT: Engine Coolant Temperature
 * VOLT: Battery voltage
 * @return The sensor value from the ECU
 */
uint8_t KWP2000::getGPS()
{
    return _GPS;
}

uint8_t KWP2000::getCLUTCH()
{
    return _CLUTCH;
}

uint8_t KWP2000::getRPM()
{
    return _RPM;
}

uint8_t KWP2000::getSPEED()
{
    if (_use_metric_system == false)
    {
        _SPEED = TO_MPH(_SPEED);
    }
    return _SPEED;
}

uint8_t KWP2000::getTPS()
{
    return _TPS;
}

uint8_t KWP2000::getSTPS()
{
    return _STPS;
}

uint8_t KWP2000::getIAP()
{
    return _IAP;
}

uint8_t KWP2000::getIAT()
{
    if (_use_metric_system == false)
    {
        _IAT = TO_FAHRENHEIT(_IAT);
    }
    return _IAT;
}

uint8_t KWP2000::getECT()
{
    if (_use_metric_system == false)
    {
        _ECT = TO_FAHRENHEIT(_ECT);
    }
    return _ECT;
}

float KWP2000::getVOLT()
{
    return _VOLT;
}

/////////////////// PRIVATE ///////////////////////

/**
 * @brief Generate and send a request to the ECU
 *
 * @param pid The PID you want to send
 * @param pid_len the length of the PID, get it with `sizeof()`
 * @param wait_to_send_all Choose to wait until the tx buffer is empty
 * @param use_delay Choose to wait at the end of the function or to do other tasks
 */
void KWP2000::sendRequest(const uint8_t pid[], const uint8_t pid_len, const uint8_t wait_to_send_all, const uint8_t use_delay)
{
    uint8_t request[20];
    uint8_t request_len = 0;
    uint8_t echo = 0;
    uint8_t header_len = 1; // minimun length

    // create the request
    // make the header
    if (_use_length_byte == true)
    {
        //we use the length byte
        request[0] = format_physical;
        request[3] = pid_len;
        header_len += 1;
    }
    else // don't use the length_byte
    {
        if (pid_len >= 64)
        {
            // we are forced to use the length byte
            request[0] = format_physical;
            request[3] = pid_len;
            header_len += 1;
        }
        else
        {
            // the length byte is "inside" the format
            request[0] = format_physical | pid_len;
        }
    }

    if (_use_target_source_address == true)
    {
        // add target and source address
        request[1] = ECU_addr;
        request[2] = OUR_addr;
        header_len += 2;
    }

    request_len = header_len + pid_len + 1; // header + request + checksum

    // add the PID
    for (uint8_t k = 0; k < pid_len; k++)
    {
        request[header_len + k] = pid[k];
    }

    // checksum
    request[request_len - 1] = calc_checksum(request, request_len - 1);

    // finally we send the request
    _elapsed_time = 0;
    for (uint8_t i = 0; i < request_len; i++)
    {
        _kline->write(request[i]);
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            if (i == 0)
            {
                _debug->println(F("\nSending\t\tEcho"));
            }
            _debug->println(request[i], HEX);
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
        if (echo != request[i] && echo != 0)
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

/**
 * @brief Listen and process the response from the ECU
 *
 * @param use_delay Choose to wait at the end of the function or to do other tasks
 */
void KWP2000::listenResponse(const uint8_t use_delay)
{
    _response_data_start = 0;
    _response_len = 0;
    memset(_response, 0, ISO_MAX_DATA);

    uint8_t masked = 0;                     // useful for bit mask operation
    uint8_t response_completed = false;     // when true no more bytes will be received
    uint32_t incoming;                      // incoming byte from the ECU
    uint8_t n_byte = 0;                     // actual length of the response, updated every times a new byte is received
    uint8_t data_to_rcv = 0;                // data to receive: bytes of the response that have to be received (not received yet)
    uint8_t data_rcvd = 0;                  // data received: bytes of the response already received
    uint32_t last_data_received = millis(); // check times for the timeout

    while ((millis() - last_data_received < ISO_T_P3_mdf) && (response_completed == false))
    {
        if (_kline->available() > 0)
        {
            incoming = _kline->read();
            _response[n_byte] = incoming;

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
            case 0: // the first byte is the formatter, with or without length bits

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

                // let's see if there are length bits
                if (_use_length_byte == true || _use_length_byte == maybe)
                {
                    masked = incoming & 0x3F; // 0b00111111
                    if (masked != 0)          // the response length is inside the formatter
                    {
                        data_to_rcv = masked;
                        if (_debug_level == DEBUG_LEVEL_VERBOSE)
                        {
                            _debug->print(F("\t- "));
                            _debug->print(data_to_rcv);
                            _debug->print(F(" data bytes coming"));
                        }

                        if (_use_length_byte == maybe)
                        {
                            _use_length_byte = false;
                            setError(EE_TEST);
                        }
                    }
                    else // the response length is in a separete byte (the 2nd or the 4th)
                    {
                        data_to_rcv = 0;
                    }
                }
                break;

            case 1: // the second byte is be the target address or the length byte or the data

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
                            // (I'm not jealous it's just curiosity :P)
                            _debug->print(F("\t- ECU is communicating with this address"));
                        }
                        setError(EE_TO);
                    }
                }
                else if (_use_target_source_address == false)
                {
                    if (data_to_rcv == 0) // it is the length byte
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

            case 3: // the fourth byte is the length byte or the data or checksum

                if (data_to_rcv == 0) // it is the length byte
                {
                    data_to_rcv = incoming;
                    if (_debug_level == DEBUG_LEVEL_VERBOSE)
                    {
                        _debug->print(F("\t- data bytes coming (HEX)"));
                    }
                }
                else // data or checksum
                {
                    if (data_to_rcv == data_rcvd) // it is the checksum
                    {
                        response_completed = true;
                        _response_len = n_byte;
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
            }         // end of the switch statement
            n_byte++; // read the next byte of the response
        }             // end of the if _kline.available()
    }                 // end of the while timeout

    if (use_delay == true)
    {
        delay(ISO_T_P3_MIN);
    }
}

/**
 * @brief Compare the given response to the correct one which should be received
 *
 * @param request_sent The request sent to the ECU
 * @return `true` if the response is correct, a `negative number` if is not
 */
int8_t KWP2000::checkResponse(const uint8_t request_sent[])
{
    if (_response[_response_data_start] == (request_ok(request_sent[0])))
    {
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->println(F("\nCorrect response from the ECU\n"));
        }
        return true;
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
        if (_debug_level == DEBUG_LEVEL_VERBOSE)
        {
            _debug->print(F("\nRequest rejected with code: "));
        }

        if (_response[_response_data_start + 1] != request_sent[0])
        {
            // this is not the request we sent!
            setError(EE_WR);
        }

        // the ECU rejected our request and gave us some useful info
        setError(EE_CR);

        switch (_response[_response_data_start + 2])
        {
        case 0x10:
            if (_debug_level == DEBUG_LEVEL_VERBOSE)
            {
                _debug->println(F("General\n"));
            }
            return -2;

        case 0x11:
            if (_debug_level == DEBUG_LEVEL_VERBOSE)
            {
                _debug->println(F("Service Not Supported\n"));
            }
            return -3;

        case 0x12:
            if (_debug_level == DEBUG_LEVEL_VERBOSE)
            {
                _debug->println(F("Sub Function Not Supported or Invalid Format\n"));
            }
            return -4;

        case 0x21:
            if (_debug_level == DEBUG_LEVEL_VERBOSE)
            {
                _debug->println(F("Busy, reapeat\n"));
            }
            setError(EE_US);
            //todo send again the request
            return -5;

        case 0x22:
            if (_debug_level == DEBUG_LEVEL_VERBOSE)
            {
                _debug->println(F("Conditions Not Correct or Request Sequence Error\n"));
            }
            return -6;
            /*
            todo
            23 routineNotComplete
            31 requestOutOfRange
            33 securityAccessDenied
            35 invalidKey
            36 exceedNumberOfAttempts
            37 requiredTimeDelayNotExpired
            40 downloadNotAccepted
            41 improperDownloadType
            42 can 'tDownloadToSpecifiedAddress
            43 can' tDownloadNumberOfBytesRequested
            50 uploadNotAccepted
            51 improperUploadType
            52 can 'tUploadFromSpecifiedAddress
            53 can' tUploadNumberOfBytesRequested
            71 transferSuspended
            72 transferAborted
            74 illegalAddressInBlockTransfer
            75 illegalByteCountInBlockTransfer
            76 illegalBlockTransferType
            77 blockTransferDataChecksumError
            */
        case 0x78:
            if (_debug_level == DEBUG_LEVEL_VERBOSE)
            {
                _debug->println(F("Request Correctly Received - Response Pending\n"));
            }
            /*
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
            setError(EE_US);
            return -7;
            /*
            79  incorrectByteCountDuringBlockTransfer
            80 - FF manufacturerSpecificCodes
            */
        default:
            if (_debug_level == DEBUG_LEVEL_VERBOSE)
            {
                _debug->println(F("Unknown error code\n"));
            }
            return -8;
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
        return -9;
    }
    // the program should never arrive here
    setError(EE_UNEX);
    return -10;
}

/**
 * @brief Set errors from `error_enum`
 *
 * @param error The error you want to set
 */
void KWP2000::setError(const uint8_t error)
{
    bitSet(_ECU_error, error);
}

/**
 * @brief Clear errors from `error_enum`
 *
 * @param error The error you want to clear
 */
void KWP2000::clearError(const uint8_t error)
{
    bitClear(_ECU_error, error);
}

/**
 * @brief Configure the K-Line behaviour from the keybytes received by the ECU
 */
void KWP2000::configureKline()
{
    // get the key bytes
    if (_response[_response_data_start + 2] != 0x8F)
    {
        setError(EE_CONFIG);
    }

    uint16_t key_bytes = _response[_response_data_start + 2] << 8 | _response[_response_data_start + 1];

    // length byte
    uint8_t AL0 = bitRead(key_bytes, 0);
    uint8_t AL1 = bitRead(key_bytes, 1);
    if (AL1 == 1 && AL0 == 1)
    {
        // both are possible, so choose the faster one
        _use_length_byte = false;
    }
    else if (AL1 == 1 && AL0 == 0)
    {
        // length byte must be present
        _use_length_byte = true;
    }
    else if (AL1 == 0 && AL0 == 1)
    {
        // length byte not needed
        _use_length_byte = false;
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
        _use_length_byte = maybe;
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
        _debug->print("Length byte:\t\t");
        _debug->println(_use_length_byte == 1 ? "Yes" : "No");
        _debug->print("Addresses bytes:\t");
        _debug->println(_use_target_source_address == 1 ? "Yes" : "No");
        _debug->print("Timing parameter:\t");
        _debug->println(_timing_parameter == 1 ? "Normal\n" : "Extended\n");
    }
}

/**
 * @brief The checksum is the sum of all data bytes modulo (&) 0xFF (same as being truncated to one byte)
 *
 * @param data All the bytes received
 * @param data_len The length of the response
 * @return The correct checksum
 */
uint8_t KWP2000::calc_checksum(const uint8_t data[], const uint8_t data_len)
{
    uint8_t cs = 0;
    for (uint8_t i = 0; i < data_len; i++)
    {
        cs += data[i];
    }
    return cs;
}

/**
 * @brief This is called when the last byte is received from the ECU
 *
 * @param received_checksum The last byte received which is the checksum
 */
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

#endif // KWP2000_cpp
