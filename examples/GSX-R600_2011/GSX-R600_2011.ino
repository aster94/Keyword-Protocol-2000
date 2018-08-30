#include "KWP2000.h"

#ifdef DEBUG_BT
#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial debug;
#else
#include <HardwareSerial.h>
HardwareSerial suzuki(2);
#endif

KWP2000 SDS(&suzuki, 115200, 22, 1);

int a = 0;

//values from the ECU
/*
uint8_t speed, rpm, tps, iap, ect, iat, stp, gear;
const uint8_t start_com[] = {0x81, 0x12, 0xF1, 0x81, 0x05};
uint8_t prova_l = 60;
uint8_t prova [60];
bool startConnection = false;
uint8_t UserSTOP = false;         //intended close of k-line
const uint16_t keepAlive = 150;   //it shouldn't be less than 40 milliseconds and have to be under 2 seconds or the comunication with the ECU would be lost
const uint16_t refreshTime = 500; //this value (milliseconds) decides how often the values from the ECU and the sensors will be updated
*/
void setup()
{
    SDS.enableDebug(&Serial, 115200);
    SDS.setDebugLevel(DEBUG_LEVEL_VERBOSE);
}
void loop()
{

    SDS.printStatus();
    if (SDS.getStatus() == false)
    {
        while (SDS.initKline() == 0)
        {
            ;
        }
    }
    else
    {
        SDS.requestSensorData();
        Serial.println(SDS.getSpeed());
        //...
    }

    if (SDS.getError() != 0)
    {
        SDS.printError();
    }
    delay(500);
}