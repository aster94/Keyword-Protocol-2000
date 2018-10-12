#include "KWP2000.h"
HardwareSerial suzuki(2);
KWP2000 ECU(&suzuki, 22, 115200);

uint8_t dealer_status = false;
char in;


void setup()
{
    ECU.enableDebug(&Serial, 115200, DEBUG_LEVEL_NONE);
    //Serial.begin(); this is not needed because we use then same serial as the debug
    ECU.enableDealerMode(25);
    
    // empty serial buffer
    while(!Serial){}
    while (Serial.available() > 0){Serial.read();}
}

void loop()
{
    ECU.printStatus();
    if (Serial.available() > 0)
    {
        in = Serial.read();
        Serial.print("User Input: ");
        Serial.println(in);

        switch (in)
        {
        case 'i':
            while (ECU.initKline() == 0){;}
            break;
        
        case 'd':
            dealer_status = !dealer_status;
            ECU.dealerMode(dealer_status);
            break;

        case 's':
            ECU.requestSensorsData();
            ECU.printSensorsData();
            break;

        case 'c':
            while (ECU.stopKline() == 0){;}
            break;
        }
        in = 0;
    }
    ECU.keepAlive();
}
