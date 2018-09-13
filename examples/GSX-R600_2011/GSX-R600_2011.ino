#include "KWP2000.h"
#include <HardwareSerial.h>
HardwareSerial suzuki(2);

KWP2000 SDS(&suzuki, 115200, 22, 1);

/*
uint8_t speed, rpm, tps, iap, ect, iat, stp, gear;  //values from the ECU
uint8_t startConnection = false;
uint8_t UserSTOP = false;         //intended close of k-line
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
    }

    if (SDS.getError() != 0)
    {
        SDS.printError();
    }
    delay(random(200, 1300));
    SDS.keepAlive(300);
    delay(200);
}