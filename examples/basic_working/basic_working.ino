#include "KWP2000.h"
HardwareSerial suzuki(2);
KWP2000 SDS(&suzuki, 22, 1, 115200);

uint8_t dealer_status = false;
char in;


void setup()
{
    SDS.enableDebug(&Serial, 115200, DEBUG_LEVEL_NONE);
    //Serial.begin(); this is not needed because we use then same serial as the debug
    SDS.enableDealerMode(25);
    
    // empty serial buffer
    while(!Serial){}
    while (Serial.available() > 0){Serial.read();}
}

void loop()
{
    SDS.printStatus();
    if (Serial.available() > 0)
    {
        in = Serial.read();
        Serial.print("User Input: ");
        Serial.println(in);

        switch (in)
        {
        case 'i':
            while (SDS.initKline() == 0){;}
            break;
        
        case 'd':
            dealer_status = !dealer_status;
            SDS.dealerMode(dealer_status);
            break;

        case 's':
            SDS.requestSensorsData();
            SDS.printSensorsData();
            break;

        case 'c':
            SDS.stopKline();
            break;
        }
        in = 0;
    }
    SDS.keepAlive();
}
