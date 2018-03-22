# the library is ready but need some cleaning, i will pubblish when it is ended
https://github.com/arduino/arduino/wiki/arduino-ide-1.5:-library-specification
https://www.arduino.cc/en/Reference/APIStyleGuide
model: https://github.com/bolderflight/MPU9250
https://help.github.com/articles/basic-writing-and-formatting-syntax/


# Keyword Protocol 2000
The KWP2000 is a communications protocol used for on-board vehicle diagnostics systems (OBD) by Suzuki (SDS), Kawasaki (KDS), Yamaha (YDS) and Honda (HDS). It is based on the ISO 14230 and uses a single line called the K-line

more info here and in the "documents" folder:
https://en.wikipedia.org/wiki/Keyword_Protocol_2000

# Usage 
please see:

# KWP2000 class

### begin
start the K-Line choosing the Serial interface, RX pin, TX pin and baudrate

### send
send a request to the ECU

### listen
listen for the response from the ECU

# Change Log - Version
0.0.3 - write a mimimum documentation

0.0.2 - library basic working 

0.0.1 - created github repo
