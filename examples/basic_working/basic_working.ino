#include "KWP2000.h"

#if defined(ARDUINO_ARCH_ESP32)
HardwareSerial bike(2); // for the ESP32 core
#elif defined(ARDUINO_ARCH_STM32)
HardwareSerial bike(PA3, PA2); // for the stm32duino core
#else
#define bike Serial2 // for the Arduino avr core
#endif

KWP2000 ECU(&bike, 13, 115200);

uint8_t dealer_status = false;
char in;
uint8_t fake_request[] = {1, 2};

void setup()
{
    ECU.enableDebug(&Serial, DEBUG_LEVEL_VERBOSE, 115200);
    //Serial.begin(); this is not needed because we use then same serial as the debug
    ECU.enableDealerMode(8);

    while (!Serial)
    {
        // wait for connection with the serial
    }
    while (Serial.available() > 0)
    {
        Serial.read(); // empty serial buffer
    }
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
            while (ECU.initKline() == 0)
            {
                ;
            }
            break;

        case 'd':
            dealer_status = !dealer_status;
            ECU.dealerMode(dealer_status);
            break;

        case 's':
            ECU.requestSensorsData();
            ECU.printSensorsData();
            break;

        case 'h':
            ECU.handleRequest(fake_request, sizeof(fake_request));
            ECU.printLastResponse();
            break;

        case 'c':
            while (ECU.stopKline() == 0)
            {
                ;
            }
            break;
        }
        in = 0;
    }
    ECU.keepAlive();
}
