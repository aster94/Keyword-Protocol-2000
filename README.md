# the library is ready but need some cleaning, i will pubblish it when it is ended
reference for myself
https://github.com/arduino/arduino/wiki/arduino-ide-1.5:-library-specification
https://www.arduino.cc/en/Reference/APIStyleGuide
https://help.github.com/articles/basic-writing-and-formatting-syntax/


# Keyword Protocol 2000
The KWP2000 is a communications protocol used for on-board vehicle diagnostics systems (OBD) by Suzuki (SDS), Kawasaki (KDS), Yamaha (YDS), Honda (HDS) and possibly more. It is based on the ISO 14230 and uses a single line called the K-line

Many IC could be used to interface the MCU with the ECU, for example the L9637, the ___ or even level shifter (not recconmended) 


more info on wikipedia and in the "extras" folder:

https://en.wikipedia.org/wiki/Keyword_Protocol_2000

## Disclaimer
if you broke your bike it's not my foult

### Tested Vehicles
-Suzuki GSX-R 600 L1 (2011)


# Usage 
generally the function return true if everything went correct, a negative number if there where any error, 0 if nothing changed

## KWP2000 class

### initKline
init

### sendRequest
this function send a request to the ECU
toSend = buffer to send (request)
sendlen = number of elements in the request

### listen
this function generate an array filled with the ECU response
meanwhile it makes sure that the data are correct and for us

###keepAlive
under 2 seconds or the comunication with the ECU would be lost

## Future Development
find tester

# Version - Change Log

0.3.0 - todo
test on the field passed
add documentation

0.2.0 - 
found bug on the esp32 core
big cleaning of the code
removed all the blocking code
added BT serial

0.1.0 - 
created github repo
