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
#include <new>

#define LEN(x) ((sizeof(x) / sizeof(0 [x])) / ((size_t)(!(sizeof(x) % sizeof(0 [x])))))

////////////// SETUP ////////////////

KWP2000::KWP2000(HardwareSerial *kline_serial, const uint8_t k_out_pin, const uint8_t model, const uint32_t kline_baudrate)
{
    _kline = kline_serial;
    _kline_baudrate = kline_baudrate;
    _k_out_pin = k_out_pin;
    _model = model;
}

void KWP2000::enableDebug(HardwareSerial *debug_serial, const uint32_t debug_baudrate, const uint8_t debug_level)
{
    _debug = debug_serial;
    _debug->begin(debug_baudrate);
    _debug_level = debug_level;

    if (_debug_level >= DEBUG_LEVEL_DEFAULT)
    {
        _debug->println("Debug enabled");
    }
}

void KWP2000::disableDebug()
{
    if (_debug_level >= DEBUG_LEVEL_DEFAULT)
    {
        _debug->println("Debug disabled");
    }
    _debug_level = DEBUG_LEVEL_NONE;
    _debug->end();
}

void KWP2000::setDebugLevel(const uint8_t debug_level)
{
    _debug_level = debug_level;
    if (_debug_level >= DEBUG_LEVEL_DEFAULT)
    {
        _debug->print("Debug level: ");
        _debug->println(debug_level);
    }
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
        _debug->print("Dealer mode: ");
        _debug->println(_dealer_mode == 1 ? "Enabled" : "Disabled");
    }
}

////////////// COMMUNICATION ////////////////

void KWP2000::sendRequest(const uint8_t to_send[], const uint8_t send_len, const uint8_t wait_to_send_all, const uint8_t use_delay)
{
    uint8_t echo = 0;
    for (uint8_t i = 0; i < send_len; i++)
    {
        _kline->write(to_send[i]);

        //too few time? todo
        if (_kline->available() > 0)
        {
            echo = _kline->read();
        }

        if (_debug_level >= DEBUG_LEVEL_VERBOSE)
        {
            if (i == 0)
            {
                _debug->println("\nSending - Echo");
            }
            _debug->print(to_send[i], HEX); //human readable values
            _debug->print("\t-\t");
            _debug->println(echo, HEX);
        }

        //check if i send the correct bytes
        if (echo != to_send[i] && echo != 0)
        {
            setError(EE_ECHO);
        }
        delay(ISO_BYTE_DELAY);
    }

    if (wait_to_send_all == true)
    {
        _kline->flush();
    }

    if (use_delay == true)
    {
        delay(ISO_DELAY_BETWEN_REQUEST);
        //todo smart delay
    }
}

//*resp pointer which point to an array where we store the data
//*resp_len              total bytes that will be received (header+response+checksum)
void KWP2000::listenResponse(uint8_t *resp, uint8_t *resp_len, const uint8_t use_delay)
{
    //assign pointer to default address if this function is called without arguments
    if (resp == nullptr)
    {
        resp = _pArray;
        if (resp == nullptr)
        {
            setError(EE_MEMORY);
            return;
        }
    }

    if (resp_len == nullptr)
    {
        resp_len = &_pArray_len;
        if (resp_len == nullptr)
        {
            setError(EE_MEMORY);
            return;
        }
    }

    //empy the array
    for (uint8_t i = 0; i < maxLen; ++i)
    {
        resp[i] = 0;
    }

    uint8_t response_completed = false;     //when true no more bytes will be received
    uint8_t incoming;                       //incoming byte from the ECU
    uint8_t n_byte = 0;                     //actual lenght of the response, updated every times a new byte is received
    uint8_t data_to_rcv = 0;                //data to receive: bytes of the response that have to be received (not received yet)
    uint8_t data_rcvd = 0;                  //data received: bytes of the response already received
    uint8_t checksum = 0;                   //the checksum
    uint32_t last_data_received = millis(); //check times for the timeout

    while ((millis() - last_data_received < ISO_MAX_SEND_TIME) && (response_completed == false))
    {
        if (_kline->available() > 0)
        {
            incoming = _kline->read();
            resp[n_byte] = incoming;

            if (_debug_level >= DEBUG_LEVEL_VERBOSE)
            {
                if (n_byte == 0)
                {
                    _debug->println("\nReceiving:");
                }
                _debug->print(incoming, HEX);
            }

            last_data_received = millis(); //reset the timer for each byte received

            //the ECU waits 10 milliseconds between sending two sequential bytes
            //we use this time to analyze what we received

            switch (n_byte)
            {
            case 0: //first byte should be an addres packet 0x80 or 0x81
                if (incoming == 0x81)
                {
                    data_to_rcv = 1; //single byte packet from the ECU
                }
                else if (incoming == 0x80)
                {
                    data_to_rcv = 0; //the packet contains more than 1 byte
                }
                else
                {
                    setError(EE_HEADER);
                    if (_debug_level >= DEBUG_LEVEL_VERBOSE)
                    {
                        _debug->print(" - unknown header");
                    }
                }
                break;

            case 1: //second byte should be the target address

                if (incoming != TESTER_addr) //the ECU is not talking with us
                {
                    setError(EE_TO);
                    //show this target addres (I'm not jealous it's just curiosity)
                    if (_debug_level >= DEBUG_LEVEL_VERBOSE)
                    {
                        _debug->print(" - ECU is communicating with this address");
                    }
                }
                break;

            case 2: //third byte should be the sender address

                if (incoming != ECU_addr) //if you have a power commander or similar it could send some request to the ECU
                {
                    setError(EE_FROM); //ignore the packet if it doesn't came from the ECU
                    //check who sent it
                    if (_debug_level >= DEBUG_LEVEL_VERBOSE)
                    {
                        _debug->print(" - this is communicating with the ECU");
                    }
                }
                break;

            case 3: //it should be the number of byte that I will receive, or the response if its a single byte packet (0x81)

                if (data_to_rcv == 1) //header 0x81
                {
                    data_rcvd = 1; //all the data has been received
                }
                else if (data_to_rcv == 0) //header 0x80
                {
                    data_to_rcv = incoming; //number of byte of data in the packet.
                }
                break;

            default: //finally the data and/or the checksum

                if (data_to_rcv != data_rcvd) //there is still data
                {
                    data_rcvd++;
                }
                else //its the checksum
                {
                    response_completed = true;
                    *resp_len = n_byte;

                    if (_debug_level >= DEBUG_LEVEL_VERBOSE)
                    {
                        _debug->println("\n\nEnd of response");
                        _debug->print("bytes received: ");
                        _debug->println(*resp_len);
                    }

                    checksum = calc_checksum(resp, *resp_len);
                    if (checksum == incoming)
                    {
                        //the checksum is correct and everything went well!
                        if (_debug_level >= DEBUG_LEVEL_VERBOSE)
                        {
                            _debug->print("Correct checksum");
                        }
                        _last_correct_response = millis();
                    }
                    else
                    {
                        //the checksum is not correct
                        if (_debug_level >= DEBUG_LEVEL_VERBOSE)
                        {
                            _debug->print("Wrong checksum, expected: ");
                            _debug->print(checksum, HEX);
                        }
                        setError(EE_CS);
                    }
                }
                break;
            }

            //new line
            if (_debug_level >= DEBUG_LEVEL_VERBOSE)
            {
                _debug->println();
            }
            n_byte++; //read the next byte of the response
        }
    }

    if (use_delay == true)
    {
        delay(ISO_DELAY_BETWEN_REQUEST);
    }
    resp_len = nullptr; //deferencing the pointer
}

void KWP2000::requestSensorsData()
{
    if (_ECU_status == false)
    {
        setError(EE_STEP);
        return;
    }

    if (_debug_level >= DEBUG_LEVEL_DEFAULT)
    {
        _debug->println("Requesting Sensors Data");
    }

#if defined(SUZUKI)

    sendRequest(request_sens, LEN(request_sens));
    listenResponse();

#elif defined(KAWASAKI)

    for (uint8_t request = 0; request < LEN(request_sens); request++)
    {
        sendRequest(request_sens[request], LEN(request_sens[request][0]));
        listenResponse();
        //increase addres of _pArray
    }

#elif defined(HONDA)

    //no idea

#endif

    //Speed
    _SPEED = _pArray[PID_SPEED] * 2;

    //RPM (Rights Per Minutes) it is split between two byte
    _RPM = _pArray[PID_RPM_H] * 10 + _pArray[PID_RPM_L] / 10;

    //TPS (Throttle Position Sensor)
    _TPS = 125 * (_pArray[PID_TPS] - 55) / (256 - 55);

    //IAP (Intake Air Pressure)
    _IAP = _pArray[PID_IAP] * 4 * 0.136;

    //ECT (Engine Coolant Temperature)
    _ECT = (_pArray[PID_ECT] - 48) / 1.6;

    //IAT (intake Air Temperature)
    _IAT = (_pArray[PID_IAT] - 48) / 1.6;

    //STPS (Secondary Throttle Position Sensor)
    _STPS = _pArray[PID_STPS] / 2.55;

    //GEAR
    _GEAR1 = _pArray[PID_GPS];
    _GEAR2 = _pArray[PID_CLUTCH];
    _GEAR3 = _pArray[PID_GEAR_3];
    //_GEAR = ?

    //GPS (Gear Position Sensor)

    /*
    other sensors

    //voltage?
    voltage = _pArray[32] * 100 / 126;

    //FUEL 40-46

    //IGN 49-52

    //STVA
    STVA = _pArray[54] * 100 / 255;

    //pair
    PAIR = _pArray[59];
    */   
}

int8_t KWP2000::initKline()
{
    if (_ECU_status == true)
    {
        if (_debug_level >= DEBUG_LEVEL_DEFAULT)
        {
            _debug->println("\nAlready connected");
        }
        return 1;
    }

    if (_sequence_started == false)
    {
        if (_debug_level >= DEBUG_LEVEL_DEFAULT)
        {
            _debug->println("\nInitialize K-line");
        }

        if (_pArray_is_allocated == false)
        {
            _pArray = new (std::nothrow) uint8_t[maxLen]; //mdf
            _pArray_is_allocated = true;
        }

        if (_pArray == nullptr)
        {
            setError(EE_MEMORY);
            return -1;
        }

        _kline->end();
        pinMode(_k_out_pin, OUTPUT);
        _sequence_started = true;
        _start_time = millis();
        _time_elapsed = 0;
    }
    _time_elapsed = millis() - _start_time;

    if (_time_elapsed < ISO_START_TIME)
    {
        digitalWrite(_k_out_pin, HIGH);
        return 0;
    }
    else if (_time_elapsed >= ISO_START_TIME && _time_elapsed < ISO_START_TIME + 25)
    {
        digitalWrite(_k_out_pin, LOW);
        return 0;
    }
    else if (_time_elapsed >= ISO_START_TIME + 25 && _time_elapsed < ISO_START_TIME + 50)
    {
        digitalWrite(_k_out_pin, HIGH);
        return 0;
    }
    else if (_time_elapsed >= ISO_START_TIME + 50)
    {
        _sequence_started = false;

        _kline->begin(_kline_baudrate);
        delay(5); //todo how much?

        sendRequest(start_com, LEN(start_com));
        listenResponse();

        if (compareResponse(start_com_ok, _pArray, LEN(start_com_ok)) == true)
        {
            if (_debug_level >= DEBUG_LEVEL_DEFAULT)
            {
                _debug->println("ECU connected");
            }
            _ECU_status = true;
            setError(EE_CLEAR);
            return 1;
        }
        else
        {
            if (_debug_level >= DEBUG_LEVEL_DEFAULT)
            {
                _debug->println("initialization failed");
            }
            _ECU_status = false;
            setError(EE_START);
            return -2;
        }
    }
    //the program should never arrive here
    setError(EE_UNEX);
    return -3;
}

int8_t KWP2000::stopKline()
{
    if (_ECU_status == false)
    {
        if (_debug_level >= DEBUG_LEVEL_DEFAULT)
        {
            _debug->println("\nAlready disconnected");
        }
        return 1;
    }

    if (_sequence_started == false)
    {
        _sequence_started = true;

        /*
        todo is there any sequence or I just close and wait?
        sendRequest(close_com, LEN(close_com));

        if (compareResponse(close_com_ok, _pArray, LEN(close_com_ok)) == true) {
        //closed without problems
        _ECU_status = false;
        setError(EE_CLEAR);
        }
        else
        {
        //not closed correctly
        _ECU_status = false;
        setError(EE_STOP);
        }
        */

        delete[] _pArray;
        _pArray_is_allocated = false;
        _last_correct_response = 0;
        _kline->end();
        _ECU_status = false;
        setError(EE_CLEAR);

        if (_debug_level >= DEBUG_LEVEL_DEFAULT)
        {
            _debug->println("ECU disconnected");
        }
        _start_time = millis();
        _time_elapsed = 0;
    }

    _time_elapsed = millis() - _start_time;

    if (_time_elapsed < 2000) //ECU ignores requests for 2 seconds after an error appear
    {
        return 0;
    }
    else
    {
        _sequence_started = false;
        return 1;
    }
}

void KWP2000::keepAlive(uint16_t time)
{
    if (_ECU_status == false) //if it is not connected it is meaningless to send a request
    {
        return;
    }

    if (millis() - _last_correct_response >= ISO_CONNECTION_LOST)
    {
        // probably the connection has been lost
        _debug->println("are you still there?");
        stopKline(); // should i try to send a request anyway?
        return;
    }

    if (time > ISO_MAX_SEND_TIME)
    {
        // prevent humans errors
        time = KEEP_ALIVE_TIME;
    }

    if (millis() - _last_correct_response <= time)
    {
        // not enough time has passed since last time we talked with the ECU
        return;
    }

    if (_debug_level >= DEBUG_LEVEL_DEFAULT)
    {
        _debug->print("\nKeeping connection alive\nLast:");
        _debug->println(millis() - _last_correct_response);
    }
#if defined(SUZUKI)
    //todo find a shorter request
    sendRequest(request_sens, LEN(request_sens));
#elif defined(KAWASAKI)
    sendRequest(request_sens[0], LEN(request_sens[0][0]));
#elif defined(HONDA)
    sendRequest(request_sens, LEN(request_sens));
#endif
    //empty the buffer
    listenResponse(); // or just a while serial read etc.
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

    if (_debug_level >= DEBUG_LEVEL_DEFAULT)
    {
        _debug->print("\n---- STATUS ----\n");
        _debug->print("Connection:\t\t");
        _debug->println(_ECU_status == 1 ? "Connected" : "Not connected");
        _debug->print("Errors:\t\t\t");
        _debug->println(_ECU_error == 0 ? "No" : "Yes");
        _debug->print("last data:\t\t");
        _debug->print(_last_correct_response == 0 ? "0" : String((millis() - _last_correct_response) / 1000.0, 2));
        _debug->println(" seconds ago");
        _debug->print("Baudrate:\t\t");
        _debug->println(_kline_baudrate);
        _debug->print("K-line TX pin:\t");
        _debug->println(_k_out_pin);
        _debug->print("Dealer pin:\t\t");
        _debug->println(_dealer_pin);
        _debug->print("Dealer mode:\t");
        _debug->println(_dealer_mode == 1 ? "Enabled" : "Disabled");

        //other stuff?

        _debug->print("\n---- ERRORS ----\n");
        if (_ECU_error == 0)
        {
            _debug->println("no errors");
        }
        else
        {
            for (uint8_t i = 0; i < EE_TOTAL; i++)
            {
                if (bitRead(_ECU_error, i) != 0)
                {
                    switch (i)
                    {
                    case EE_CLEAR:
                        // skip
                        break;
                    case EE_START:
                        _debug->println("unable to start comunication");
                        break;
                    case EE_STOP:
                        _debug->println("unable to stop comunication");
                        break;
                    case EE_TO:
                        _debug->println("data is not for us");
                        break;
                    case EE_FROM:
                        _debug->println("data don't came from the ECU");
                        break;
                    case EE_CS:
                        _debug->println("checksum error");
                        break;
                    case EE_ECHO:
                        _debug->println("echo error");
                        break;
                    case EE_MEMORY:
                        _debug->println("memory error");
                        break;
                    case EE_UNEX:
                        _debug->println("unexpected error");
                        break;
                    case EE_HEADER:
                        _debug->println("unexpected header");
                        break;
                    case EE_STEP:
                        _debug->println("Tried to send a request without connection");
                        break;
                    default:
                        _debug->print("did I forget any enum?");
                        _debug->println(i);
                        break;
                    }
                }
            }
        }
        _debug->print("---- ------- ----\n\n");
        _last_status_print = millis();
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

    if (_debug_level >= DEBUG_LEVEL_NONE)
    {
        _debug->print("---- SENSORS ----\n");
        _debug->print("Calculated ");
        _debug->print((millis() - _last_correct_response) / 1000.0);
        _debug->println(" seconds ago");
        _debug->print("Gear:\t");
        _debug->println(_GEAR);
        _debug->print("RPM:\t");
        _debug->println(_RPM);
        _debug->print("Speed:\t");
        _debug->println(_SPEED);
        _debug->print("TPS:\t");
        _debug->println(_TPS);
        _debug->print("IAP:\t");
        _debug->println(_IAP);
        _debug->print("ECT:\t");
        _debug->println(_ECT);
        _debug->print("STPS:\t");
        _debug->println(_STPS);
        _debug->print("IAT:\t");
        _debug->println(_IAT);
        //_debug->print(":\t");_debug->println();

        _debug->print("_GEAR1:\t");
        _debug->println(_GEAR1, BIN);
        _debug->print("_GEAR2:\t");
        _debug->println(_GEAR2, BIN);
        _debug->print("_GEAR3:\t");
        _debug->println(_GEAR3, BIN);

        _debug->print("---- ------- ----\n");
        _last_data_print = millis();
    }
}

void KWP2000::printLastResponse(uint8_t *resp, uint8_t **resp_len)
{
    
    uint8_t save = false;
    if (resp != nullptr && resp_len != nullptr){
        save = true;
        *resp_len = &_pArray_len;
    }

    _debug->println("Last Response from the ECU:");
    for (uint8_t n = 0; n < _pArray_len; n++)
    {
        _debug->println(_pArray[n],HEX);

        // save response to a given array
        if (save){
            resp[n] = _pArray[n];
        }
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
    setError(EE_CLEAR);
}

uint8_t KWP2000::getGear()
{
    return _GEAR;
}

uint8_t KWP2000::getRPM()
{
    return _RPM;
}

uint8_t KWP2000::getSpeed()
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

uint8_t KWP2000::getECT()
{
    return _ECT;
}

uint8_t KWP2000::getSTPS()
{
    return _STPS;
}

/////////////////// PRIVATE ///////////////////////

void KWP2000::setError(const uint8_t error)
{
    if (error == EE_CLEAR)
    {
        _ECU_error = 0;
    }
    else
    {
        bitSet(_ECU_error, error);
    }
}

//Checksum is the sum of all data bytes modulo (&) 0xFF
//(same as being truncated to one byte)
uint8_t KWP2000::calc_checksum(const uint8_t data[], const uint8_t cs_len)
{
    uint8_t cs = 0;

    for (uint8_t i = 0; i < cs_len; i++)
    {
        cs += data[i];
    }
    return cs;
}

int8_t KWP2000::compareResponse(const uint8_t expected_response[], const uint8_t received_response[], const uint8_t expected_response_len)
{
    uint8_t coincident = -1;
    coincident = memcmp(expected_response, received_response, expected_response_len);

    if (coincident == 0)
    {
        return 1;
    }
    else
    {
        setError(EE_CS);
        return -1;
    }
}
