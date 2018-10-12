#include "KWP2000.h"
HardwareSerial suzuki(2);
KWP2000 SDS(&suzuki, 22, 1, 115200);

uint8_t dealer_status = false;
char in;
uint8_t *pData;
uint8_t *data_len;
uint8_t your_request[] = {0x80, 0x12, 0xF1, 0x02, 0x21, 0x08, 0xAE};


void setup()
{
    pData = new uint8_t [60];
    SDS.enableDebug(&Serial, 115200);
    //Serial.begin(); this is not needed because we use then same serial as the debug
    SDS.setDebugLevel(DEBUG_LEVEL_VERBOSE);
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

        case 'c':
            SDS.stopKline();
            break;

        case 's':
            SDS.requestSensorsData();
            break;

        case 'd':
            dealer_status = !dealer_status;
            SDS.dealerMode(dealer_status);
            break;
        
        case 'r':
            SDS.sendRequest(your_request, sizeof(your_request));
            SDS.listenResponse();
            SDS.printLastResponse(pData, &data_len);
            for (int a = 0; a< *data_len;a++){
                Serial.println(pData[a],HEX);
            }
            break;

        case 'p':
            // you need to run requestSensorsData before
            SDS.printSensorsData(true);
        }
        in = 0;
    }
    SDS.keepAlive();
}
