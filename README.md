github text formatting
https://help.github.com/articles/basic-writing-and-formatting-syntax/


# Keyword Protocol 2000
The KWP2000 is a communications protocol used for on-board vehicle diagnostics systems (OBD) by Suzuki (SDS), Kawasaki (KDS), Yamaha (YDS), Honda (HDS) and possibly more. It is based on the ISO 14230 and uses a single line called the K-line

more info on [Wikipedia](https://en.wikipedia.org/wiki/Keyword_Protocol_2000/) and in the extras folder

Many IC could be used to interface a general MCU with the ECU, for example the L9637, the ___ or even level shifter (not recconmended)

### Tested Vehicles
- Suzuki GSX-R 600 L1 (2011)


## Disclaimer
if you broke your bike it's not my fault


# Usage 
Generally the function return `true` if everything went correct, a `negative number` if there where any error, `0` if nothing changed

## KWP2000 class

### initKline
Initialize the comunication with the ECU

### stopKline
Close the comunication with the ECU

### sendRequest
This function send a request to the ECU

### listen
This function generate an array filled with the ECU response, meanwhile it makes sure that the data are correct and for us

### keepAlive
After 2 seconds of inactivity the comunication with the ECU would be lost so we keep it alive

### get*

### print*

## Future Development
find more tester

## Version - Change Log

##### 1.0.0 - 
- version promoted for the arduino IDE, no changes

##### 0.3.0 - 
- ESP32 team corrected the bug, thank you!
- test on the field passed
- add documentation

##### 0.2.0 - Aug 30, 2018
- found bug in the hardwareSerial class of the ESP32 core
- big cleaning of the code
- removed all the blocking code
- added BT serial

##### 0.1.0 - Mar 22, 2018
- created github repo
