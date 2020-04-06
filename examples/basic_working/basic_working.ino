#include <Arduino.h>
#include "KWP2000.h"

// Different core have different way to use a Serial port
#if defined(ARDUINO_ARCH_ESP32)
HardwareSerial bike(2);
#define TX_PIN 17
#elif defined(ARDUINO_ARCH_STM32)
HardwareSerial bike(PA3, PA2); // RX and TX
#define TX_PIN PA2
#else
#define bike Serial3
#define TX_PIN 14
#endif

#define debug Serial

// Replace YOUR_MOTORBIKE with SUZUKI, KAWASAKI, YAMAHA or HONDA
KWP2000 ECU(&bike, TX_PIN, YOUR_MOTORBIKE);

void setup()
{
    ECU.enableDebug(&debug, DEBUG_LEVEL_VERBOSE, 115200);
    //Serial.begin(); this is not needed because we use then same serial as the debug
    //ECU.enableDealerMode(pin); // This is available only on Suzuki bikes
}

void loop()
{
    if (debug.available() > 0)
    {
        char in = debug.read();
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
            ECU.setDealerMode(!ECU.getDealerMode());
            break;

        case 't':
            ECU.readTroubleCodes();
            break;

        case 's':
            ECU.requestSensorsData();
            ECU.printSensorsData();
            break;

        case 'p':
            ECU.printStatus();
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
