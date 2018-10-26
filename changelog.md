# Version - Change Log

## Future Development
- test on other bikes
- discover new PIDs

#### 1.0.0 - soon
- road test passed :D
- version promoted for pubblication on the arduino library manager

#### 0.5.0 - Oct 26, 2018
- completely rewritten the sendRequest, listenResponse and checkResponse
- more compliant with the ISO14230
- removed std::notrow since [arduino seems to not support it](https://github.com/arduino/ArduinoCore-avr/issues/47)
- added board/MCU specific flags at compile time
- added ability to read the keybytes and the timing parameters
- split the readme in multiple files for readability
- many improvements in the python ECU emulator

#### 0.4.0 - Oct 12, 2018
- made two examples sketches
- added ECU_Emulator written in python
- merged printStatus and printError
- added printSensorData
- added printLastResponse
- added new errors and better error handling
- added documentation
- review all the code
- added more resources

#### 0.3.0 - Sep 13, 2018
- solved the bug with FreeRTOS
- moved the BT serial in a fork
- added wonderful code to manage comunication errors

#### 0.2.0 - Aug 30, 2018
- problem with FreeRTOS 
- big cleaning of the code
- removed all the blocking code
- added BT serial

#### 0.1.0 - Mar 22, 2018
- created github repo
