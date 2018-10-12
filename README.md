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

constructor
at pc use a different pin then the real tx

### initKline
Initialize the comunication with the ECU

### stopKline
Close the comunication with the ECU

### sendRequest
This function send a request to the ECU
flush() takes about 15 microseconds

### listen
This function generate an array filled with the ECU response, meanwhile it makes sure that the data are correct and for us
Calling send or listen without inizializing the kline could bring to unpredictable crashes

### keepAlive
After 2 seconds of inactivity the comunication with the ECU would be lost so we keep it alive
it shouldn't be less than 40 milliseconds and have to be under 2 seconds or the comunication with the ECU would be lost

### get*

### print*

## Future Development
find more tester

## Version - Change Log

##### 0.5.0 - ?
- road test passed :D
- added documentation

##### 0.4.0 - Oct 9, 2018
- made a decent example sketch
- added ECU_Emulator written in python
- merged printStatus and printError
- added printSensorData
- review all the code
- added new errors and better error handling

##### 0.3.0 - Sep 13, 2018
- solved the bug with FreeRTOS
- moved the BT serial in a fork
- added wonderful code to manage comunication errors

##### 0.2.0 - Aug 30, 2018
- problem with FreeRTOS 
- big cleaning of the code
- removed all the blocking code
- added BT serial

##### 0.1.0 - Mar 22, 2018
- created github repo
