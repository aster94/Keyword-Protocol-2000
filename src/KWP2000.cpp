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

const uint8_t maxLen = 60; //maximum lenght of a response from the ecu
uint8_t storeArray[maxLen]; //array where I store the response from the ECU before the extraction of the data


int8_t KWP2000::test(){
}

////////////// SETUP ////////////////

KWP2000::KWP2000(HardwareSerial* kline_serial, const uint32_t kline_baudrate, const uint8_t k_out_pin, const uint8_t model){
  _kline = kline_serial;
  _kline_baudrate = kline_baudrate;
  _k_out_pin = k_out_pin;
  _model = model;
}

void KWP2000::enableDebug(HardwareSerial* debug_serial, const uint32_t debug_baudrate, const uint8_t debug_level){
  _debug = debug_serial;
  _debug->begin(debug_baudrate);
  _debug_level = debug_level;
}

void KWP2000::disableDebug(){
  _debug->end();
}

void KWP2000::setDebugLevel(const uint8_t debug_level){
  _debug_level = debug_level;
}

void KWP2000::enableDealerMode(const uint8_t dealer_pin){
  _dealer_pin = dealer_pin;
}

void KWP2000::dealerMode(const uint8_t dealer_mode){
  digitalWrite(_dealer_pin, dealer_mode);
}

////////////// COMMUNICATION ////////////////

int8_t KWP2000::initKline(){

  //allocate memory

  pinMode(_k_out_pin, OUTPUT);
  delay(5);

  digitalWrite(_k_out_pin, HIGH);
  delay(2000);
  digitalWrite(_k_out_pin, LOW);
  delay(25);
  digitalWrite(_k_out_pin, HIGH);
  delay(25);

  _kline->begin(_kline_baudrate);
  delay(5);

  sendRequest(startCom, sizeof(startCom));
  listenResponse(storeArray);

  if (compareResponse (startCom_ok, storeArray, sizeof(startCom_ok)) == true) {

    //ECU is connected!
    _ECUstate = true;
    _ECUerror = 0;
    return 1;

  } else {

    //failed initialization
    _ECUstate = false;
    bitSet (_ECUerror, EE_START);
    return -1;
  }
}

int8_t KWP2000::stopKline(){

  //free memory
  /*
    is there any sequence or I just close and wait?
    sendRequest(closeCom, sizeof(closeCom));

    if (compareResponse(closeCom_ok, sizeof(closeCom_ok))) {
      //closed without problems
      ECUconnected = false;
      ECUerror = 0:
      ConnectionLED_OFF
      ECUerrorLED_OFF

    } else {

    //not closed correctly
    ECUconnected = false;
    ECUerror = 2;
    ECUerrorLED_ON
    ConnectionLED_OFF
    }
  */

  _kline->end();

  _ECUstate = false;
  _ECUerror = 0;
  //ConnectionLED_OFF
  //ECUerrorLED_OFF

  //delay(3000);  //ECU ignores requests for 2 seconds after an error appear todo
  return 1;
}

void KWP2000::requestSensorData(){
  //if suzuki
  sendRequest(requestSens,sizeof(requestSens));
  listenResponse(storeArray);
  processSensorData(storeArray, sizeof(storeArray));
  //return 1,-1
  //if kawasaki
  //...
}

//this function send a request to the ECU
//toSend = buffer to send (request)
//sendlen = number of elements in the request
int8_t KWP2000::sendRequest(const uint8_t toSend[], const uint8_t sendlen) {

  for (uint8_t i = 0; i < sendlen; i++) {

    _kline->write(toSend[i]);

    if (_debug_level == DEBUG_LEVEL_VERBOSE) {
        if (i == 0) _debug->println("Sending:");
        _debug->println(toSend[i], HEX);             //human readable values
    }

    delay(ISO_BYTE_DELAY);

    if (_debug_level == DEBUG_LEVEL_VERBOSE) { 
    //check if i send the correct bytes
        if (toSend[i] != _kline->read()) {
          bitSet (_ECUerror, EE_ECHO);
        }
    }
  }

  //delay(ISORequestDelay); I know that I should write it
  //but I would rather to use this time to accomplish other task
  return 1;
}

void KWP2000::listenResponse(uint8_t arr[]) {

  //this function generate an array filled with the ECU response
  //meanwhile it makes sure that the data are correct and for us
  //todo use pointer, more elegant and better

  uint8_t responsebyte = 0;       //actual lenght of the response, updated every times a new byte is received
  uint8_t lastbyte = 0;           //#n of the last byte from the ECU
  uint8_t in;                     //incoming byte
  uint8_t dataToRcv = 0;          //bytes of the response that have to be received (not received yet)
  uint8_t dataRcvd = 0;           //bytes of the response already received
  uint8_t checksum = 0;           //the checksum
  boolean forMe = false;          //initialize value, it is to be sure that the packet is address to us
  boolean fromECU = false;        //initialize value, it is to be sure that the packet is from the ECU

  uint8_t toReceive[maxLen];         //maxLen should be 61 in suzuki
  memset(toReceive, 0, maxLen);   //empy the array

  uint32_t startTime = millis();     //check times for the timeout

  while (((millis() - startTime) < ISO_MAX_SEND_TIME) && lastbyte == 0) {

    if (_kline->available() > 0) {

      in = _kline->read();
      toReceive[responsebyte] = in;

  if (_debug_level == DEBUG_LEVEL_VERBOSE) { 
    if (responsebyte == 0) Serial.println("Receiving:");
    Serial.println(in, HEX);
  }

      startTime = millis(); //reset the timer for each byte received

      //the ECU waits 10 milliseconds between two byte so analize the byte that we received

      switch (responsebyte) {

        case 0: //first byte should be an addres packet 0x80 or 0x81

          if (in == 0x81) {
            dataToRcv = 1; //single byte packet from the ECU
          }
          else if (in == 0x80) {
            dataToRcv = 0;  //the packet contains more than 1 byte
          }

          break;

        case 1:  //second byte should be the target address

          if (in == TESTERaddr) {
            forMe = true;
          }
          else {
            bitSet (_ECUerror, EE_TO);        //ignore the packet if it isn't for us
            forMe = false;
          }
          //show this target addres (I'm not jealous it's just curiosity)
          //Serial << "addres: " << in << newl;

          break;

        case 2: //third byte should be the sender address

          if (in == ECUaddr) {
            fromECU = true;
          }
          else {
            bitSet (_ECUerror, EE_FROM);       //ignore the packet if it doesn't came from the ECU
            fromECU = false;
          }
          //if you have a power commander or similar it could send some request to the ECU

          break;

        case 3: //it should be the number of byte that I will receive, or the response if its a single byte packet (0x81)

          if (forMe == true && fromECU == true) {
            if (dataToRcv == 1) {             //header 0x81
              dataRcvd = 1;
            } else if (dataToRcv == 0) {      //header 0x80
              dataToRcv = in;                 //number of byte of data in the packet.
            }
          }

          break;

        default:  //finally the data and/or the checksum

          if (forMe == true && fromECU == true) {

            if (dataToRcv != dataRcvd) {
              dataRcvd++;
            }
            else {                          //its the checksum
              lastbyte = responsebyte;
              checksum = calc_checksum(toReceive, responsebyte);
              if (checksum == in) {
                ; //the checksum is correct and everything went well!
              }
              else {
                bitSet (_ECUerror, EE_CS);  //the checksum is not correct
              }
            }
          }
          break;
      }
      responsebyte++;          //read the next byte of the response
    }
  }

  for (uint8_t i = 0; i <= lastbyte; i++) {
    storeArray[i] = toReceive[i];           //populate the array
  }

  //delay(ISORequestDelay); as before we use this time to do other things


  if (_debug_level == DEBUG_LEVEL_VERBOSE){
    Serial.println("\nFrom the ECU:");
    Serial.print("number of bytes\t\t"); Serial.println(lastbyte);
    Serial.print("expected checksum\t"); Serial.println(checksum, HEX);
    Serial.println("");

    Serial.println("Received:");
    for (int i = 0; i < lastbyte; i++) {
      Serial.print(i); Serial.print("\t"); Serial.println(storeArray[i], HEX);
    }
    Serial.println();
  }
}

void KWP2000::processSensorData(const uint8_t data[], const uint8_t dataLen) {

  //Speed
  _SPEED = data[PID_SPEED] * 2;

  //RPM (Rights Per Minutes) it is split between two byte
  _RPM = data[PID_RPM_H] * 10 +  data[PID_RPM_L] / 10;
  //RPM = (data[PID_RPM_H]*256 + data[PID_RPM_L]) / 2.56 which one?

  //TPS (Throttle Position Sensor)
  _TPS = 125 * (data[PID_TPS] - 55) / (256 - 55);

  //IAP (Intake Air Pressure)
  _IAP = data[PID_IAP] * 4 * 0.136;

  //ECT (Engine Coolant Temperature)
  _ECT = (data[PID_ECT] -  48) / 1.6;

  //IAT (intake Air Temperature)
  _IAT = (data[PID_IAT] -  48) / 1.6;

  //STPS (Secondary Throttle Position Sensor)
  _STPS = data[PID_STPS] / 2.55;

  //GEAR
  _GEAR1 = data[PID_GPS];
  _GEAR2 = data[PID_CLUTCH];
  _GEAR3 = data[PID_GEAR_3];
  //_GEAR = ?

  //GPS (Gear Position Sensor)

  /*
    other sensors

    //voltage?
    voltage = data[32] * 100 / 126;

    //FUEL 40-46

    //IGN 49-52

    //STVA
    STVA = data[54] * 100 / 255;

    //pair
    PAIR = data[59];
  */

  if (_debug_level > DEBUG_LEVEL_VERBOSE){
    Serial.println("PROCESS data");
    Serial.print("speed\t"); Serial.println(_SPEED);
    Serial.print("RPM\t"); Serial.println(_RPM);
    Serial.print("TPS\t"); Serial.println(_TPS);
    Serial.print("IAP\t"); Serial.println(_IAP);
    Serial.print("ECT\t"); Serial.println(_ECT);
    Serial.print("IAT\t"); Serial.println(_IAT);
    Serial.print("STPS\t"); Serial.println(_STPS);
    Serial.print("GEAR1\t"); Serial.println(_GEAR1, BIN);
    Serial.print("GEAR2\t"); Serial.println(_GEAR2, BIN);
    Serial.print("GEAR3\t"); Serial.println(_GEAR3, BIN);
    Serial.print("MORE...\t");
    Serial.println("");
  }
}

void KWP2000::keepAlive(){

}


/////////////////// PRINT and GET ///////////////////////

void KWP2000::printStatus(){
  
  //how to check if debug enabled?
   if (_debug_level >= DEBUG_LEVEL_DEFAULT) {
    _debug->print("\n---- STATUS ----\n");
    _debug->print("Connection:\t");
    _debug->println(_ECUstate == 1 ? "Connected" : "Not connected");
    _debug->print("Errors:\t\t");
    _debug->println(_ECUerror != 0 ? "Yes" : "No");
    //serial ports used
    //other stuff

    _debug->print("---- ------- ----\n");
   }
}

void KWP2000::printError(){
  if (_debug_level >= DEBUG_LEVEL_DEFAULT) {
     _debug->print("\n---- ERRORS ----\n");
    if (_ECUerror == 0){
      _debug->println("no errors");
    } else {
      for (uint8_t i = 0; i < 8; i++){
        if (bitRead(_ECUerror, i) != 0){
          switch(i){
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
            default:
              _debug->println("???");
              break;
          }
        }
      }
    }
    _debug->print("---- ------- ----\n");
  }
}

int8_t KWP2000::getState(){
  return _ECUstate;
}

int8_t KWP2000::getError(){
  return _ECUerror;
}

void KWP2000::resetError(){
  _ECUerror = 0;
}

uint8_t KWP2000::getGear(){
  return _GEAR;
}

uint8_t KWP2000::getRPM(){
  return _RPM;
}

uint8_t KWP2000::getSpeed(){
  return _SPEED;
}

uint8_t KWP2000::getTPS(){
  return _TPS;
}

uint8_t KWP2000::getIAP(){
  return _IAP;
}

uint8_t KWP2000::getECT(){
  return _ECT;
}

uint8_t KWP2000::getSTPS(){
  return _STPS;
}


/////////////////// PRIVATE ///////////////////////

//Checksum is the sum of all data bytes modulo (&) 0xFF
//(same as being truncated to one byte)
uint8_t KWP2000::calc_checksum(const uint8_t data[], const uint8_t len) {
  uint8_t cs = 0;

  for (uint8_t i = 0; i < len; i++) {
    cs += data[i];
    Serial.println(cs);
  }
  return cs;
}

int8_t KWP2000::compareResponse (const uint8_t expectedResponse[], const uint8_t receivedResponse[], const uint8_t expectedResponseLen) {
  uint8_t coincident = 0;
  coincident = memcmp(expectedResponse, receivedResponse, expectedResponseLen);

  if (coincident == 0) {
    return 1;
  } else {
    bitSet (_ECUerror, EE_CS);
    return -1;
  }
}