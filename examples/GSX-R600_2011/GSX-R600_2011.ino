#include "KWP2000.h"

KWP2000 SDS(&Serial3, 115200, 13, 1);
bool startConnection = false;

uint8_t UserSTOP = false;   //intended close of k-line
const uint16_t keepAlive = 150;  //it shouldn't be less than 40 milliseconds and have to be under 2 seconds or the comunication with the ECU would be lost
const uint16_t refreshTime = 500;     //this value (milliseconds) decides how often the values from the ECU and the sensors will be updated

//values from the ECU
uint8_t speed, rpm, tps, iap, ect, iat, stp, gear;

void setup() {

  SDS.enableDebug (&Serial, 115200);
  SDS.setDebugLevel(DEBUG_LEVEL_DEFAULT);

}

void loop() {
  SDS.printStatus();
  SDS.setDebugLevel(DEBUG_LEVEL_VERBOSE);

  if (SDS.getState() == false){
    // trying to connect...
    SDS.initKline();
  }

  if (SDS.getState() == true){
    //connected!
    SDS.requestSensorData();
    Serial.println(SDS.getSpeed());
    //...
  }
  
  if (SDS.getError() != 0){
    SDS.printError();
    SDS.setDebugLevel(DEBUG_LEVEL_NONE);
  }
}