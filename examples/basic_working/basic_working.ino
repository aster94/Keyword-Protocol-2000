#include <Arduino.h>
#include "KWP2000.h"

#if defined(ARDUINO_ARCH_ESP32)
HardwareSerial bike(2); // for the ESP32 core
#elif defined(ARDUINO_ARCH_STM32)
HardwareSerial bike(PA3, PA2); // for the stm32duino core
#else
#define bike Serial3 // for the Arduino avr core
#endif

#define debug Serial
#define TX_PIN 14

KWP2000 ECU(&bike, TX_PIN);

uint8_t dealer_status = false;
char in;
uint8_t fake_request[] = {1, 2};

void setup()
{
    ECU.enableDebug(&debug, DEBUG_LEVEL_VERBOSE, 115200);
    //Serial.begin(); this is not needed because we use then same serial as the debug
    //ECU.enableDealerMode(8);

    while (!debug)
    {
        // wait for connection with the serial
    }
    while (debug.available() > 0)
    {
        debug.read(); // empty serial buffer
    }
}

void loop()
{
    //ECU.printStatus();
    if (debug.available() > 0)
    {
        in = debug.read();
        debug.print("User Input: ");
        debug.println(in);

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

        case 'D':
            ECU.readTroubleCodes();
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
