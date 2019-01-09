#include "KWP2000.h"

#if defined (ARDUINO_ARCH_ESP32)
HardwareSerial bike(2); // for the ESP32 core
#elif defined(ARDUINO_ARCH_STM32)
HardwareSerial bike(PA3,PA2); // for the stm32duino core
#else
#define bike Serial2 // for the Arduino avr core
#endif

KWP2000 ECU(&bike, 17);

uint8_t dealer_status = false;
char in;
uint8_t *pData;
uint8_t data_len;
uint8_t your_request[] = {0x80, 0x12, 0xF1, 0x02, 0x21, 0x08, 0xAE};


void setup()
{
    ECU.enableDebug(&Serial, 115200);
    //Serial.begin(); this is not needed because we use then same serial as the debug
    ECU.setDebugLevel(DEBUG_LEVEL_VERBOSE);
    ECU.enableDealerMode(18);
    
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
            while (ECU.initKline(&pData) == 0){;}
            break;

        case 'c':
            while (ECU.stopKline(&pData, &data_len) == 0){;}
            break;

        case 's':
            ECU.requestSensorsData();
            ECU.printSensorsData();
            break;

        case 'd':
            dealer_status = !dealer_status;
            ECU.dealerMode(dealer_status);
            break;
        
        case 'r':
            ECU.sendRequest(your_request, sizeof(your_request));
            ECU.listenResponse(pData, &data_len);
            Serial.print("Receiving bytes:\t"); Serial.println(data_len);
            for (int a = 0; a < data_len; a++){
                Serial.println(pData[a], HEX);
            }
            break;
        }
        in = 0;
    }
    ECU.keepAlive();
}
