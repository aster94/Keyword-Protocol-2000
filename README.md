# Keyword Protocol 2000
The [KWP2000](https://en.wikipedia.org/wiki/Keyword_Protocol_2000/) is a communications protocol used for [on-board vehicle diagnostics systems (OBD)](https://en.wikipedia.org/wiki/On-board_diagnostics) by Suzuki (SDS), Kawasaki (KDS), Yamaha (YDS), Honda (HDS) and more. It is standardized by the [ISO 14230](https://www.iso.org/obp/ui/#iso:std:iso:14230:-1:ed-2:v1:en), which is very similar to the [ISO 9141-2](https://www.iso.org/obp/ui/#iso:std:iso:9141:-2:ed-1:v1:en). Both uses a single line called the K-line through it we send some Parameter IDs, knowns as PIDs, to the [Electronic Control Unit (ECU)](https://en.wikipedia.org/wiki/Electronic_control_unit). This way we have total control on the ECU: we are able to ask for sensors data (such as RPM, gear, speed, temperatures, etc.), see error codes (clear them), upload/download and so on


### Hardware
Any microcontroller (MCU) could be used. Many IC could be used as interface, for example:
- L9637
- MC33660
- MC33199
- operational amplifiers/level shifter/transistors (**not recommended**)

### Tested Vehicles
- Suzuki GSX-R 600 L1 (2011)

# Usage
### Hardware
You have to find the K-line of your bike. In suzuki bikes it's under the rider's seat. Connect the Kline, Vcc and Gnd from the motorbikes

Dealer Mode: if you wish also to be able to enter easily into this mode add an optocoupler with a ~330omh resistor between the MCU and the dealer pin

**I will post pictures and a schematics*

### Software
First of all go to [PIDs.h](/src/PIDs.h) and decomment (delete the `//` symbols) your motorbike, then upload any of the the [examples](/examples/)
I also made a ECU Emulator written in python for the development of new functions/tests

## Installation
Simply add this from the Arduino Library Manager or download this repository and add it to your library folder

## Function Description
Generally the functions return `true` if everything went fine, a `negative number` if there where any error, `false` if nothing changed

# Documentation

## This is changing a lot, until versione 1.0.0 I won't update this documentation again so ask me or read the code

### KWP2000(HardwareSerial *kline_serial, const uint8_t k_out_pin, const uint32_t kline_baudrate = 10400)
**Constructor:** choose the serial port which will be used to talk with the ECU and the TX pin of this serial

*Optional:* baudrate of the kline

Example:
> KWP2000 ECU (Serial2, 10);



## Setup
### enableDebug(HardwareSerial *debug_serial, const uint32_t debug_baudrate = 115200, const uint8_t debug_level = DEBUG_LEVEL_DEFAULT)
**Choose and enable the debug:** select the serial port which will be used to print useful messages

*Optional:* baudrate, verbosity

Example:
> ECU.enableDebug(Serial);


### setDebugLevel(const uint8_t debug_level)
**Change the verbosity of the debug messages:** you could choose between:
- DEBUG_LEVEL_DEFAULT   only the most useful messages
- DEBUG_LEVEL_VERBOSE   print almost everythings happends between your MCU and the ECU

Example:
> ECU.setDebugLevel(DEBUG_LEVEL_VERBOSE);


### disableDebug()
**Kill the debug messages**

Example:
> ECU.disableDebug();


### enableDealerMode(const uint8_t dealer_pin)
**Enable the dealer mode:** choose the pin which trigger the dealer mode

Example:
> ECU.enableDealerMode(14);


### dealerMode(const uint8_t dealer_mode)
**Enable/disable the dealer mode**

Example:
> ECU.dealerMode(true);



## Comunation - Basic
### initKline()
**Initialize the comunication with the ECU:** returns `false` until the inizialization sequence is not completed, `true` when it is completed and correct, a `negative number` if something has gone wrong. A private array would be create from this function to store the response from the ECU

*Optional:* select a different pointer which will contain the responses. See [*advanced_working.ino*](/examples/advanced_working/advanced_working.ino)

Example:
> while(ECU.initKline() == 0){;};


### stopKline()
**Close the comunication with the ECU:** returns `false` until the end sequence is not completed, `true` when it is completed

*Optional:* if selected above a different memory address this would be deallocated

Example:
> while(ECU.stopKline() == 0){;};


### keepAlive(uint16_t time = 1000)
**Keep the KLine active:** after 2 seconds of inactivity the KLine closes automatically, this function should be at the end of you *loop()* or anyway called often

*Optional:* choose how often this function will be called

Example:
> loop() {/*your code*/ ECU.keepAlive();}


### requestSensorsData()
**Send a request and listen it's response:** this function does everything you need

Example:
> ECU.requestSensorsData();



## Comunation - Advanced
### sendRequest(const uint8_t to_send[], const uint8_t send_len, const uint8_t wait_to_send_all = false, const uint8_t use_delay = false)
**Send a request to the ECU - Use this only if you know what are you doing:** select the *byte/uint8_t* array which will be sent and the size of this array

*Calling this without inizializing the kline could bring to unpredictable crashes*

*Optional:* wait until we empty the TX buffer (this takes about 15 microseconds fore a 8 byte request) and wait before making the next request

Example:
> ECU.sendRequest(start_sequence, sizeof(start_sequence));


### listenResponse(uint8_t *resp = nullptr, uint8_t *resp_len = nullptr, const uint8_t use_delay = false)
**Listen the response from the ECU - Use this only if you know what are you doing:** this function fill an array with the ECU response, meanwhile it makes sure that the data are correct and for us

*Calling this without inizializing the kline could bring to unpredictable crashes*

*Optional:* change the memory addres where you want the response to be stored, wait before making the next request

Example:
> ECU.listenResponse();



## Print and Get Methods
### printStatus(uint16_t time = 2000)
**Print some useful info and error messages:** 
```
---- STATUS ----
Connection:		Connected
Errors:			No
last data:		0.37 seconds ago
Baudrate:		115200
K-line TX pin:	    22
Dealer pin:		25
Dealer mode:	    Disabled

---- ERRORS ----
no errors
---- ------- ----
```
*Optional:* choose how often this function will be called

Example:
> ECU.printStatus();


### printSensorsData(uint16_t time = 1)
**Print the sensors data:** needs to be called after *requestSensorsData()*

*Optional:* choose how often this function will be called

Example:
> ECU.printSensorsData();


### printLastResponse()
**Print the last response received by the ECU**

Example:
> ECU.printLastResponse();


### getStatus()
**Get the connection status of the ECU:** returns `true` if connected, `false` if not

Example:
> ECU.getStatus();


### getError()
**Get a general error:** this function will just return `true` if there is no error, `-1` if there is any error. The library handles 12+ possible error codes, see them in *printStatus()*

Example:
> ECU.getError();


### resetError()
**Remove all error codes**

Example:
> ECU.resetError();


### get*
**get any sensor value:** return the value of the sensor, actually it's possibile with:
- GPS: Gear Position Sensor
- RPM: Right Per Minutes
- SPEED: speed of the bike
- TPS: Throttle Position Sensor
- IAP: Intake Air Pressure
- IAT: Intake Air Temperature
- ECT: Engine Coolant Temperature
- STPS: Secondary Throttle Position Sensor
- *more are coming*

Example:
> ECU.getRPM();



## Disclaimer
I do not assume and hereby disclaim any liability to any party for any damage to proprierty or person


## Influence:
- [ECU Hacking forum](https://ecuhacking.activeboard.com/): 
    - https://ecuhacking.activeboard.com/t56234221/kds-protocol

- [Arduino forum](https://forum.arduino.cc/)
    - https://forum.arduino.cc/index.php?topic=236092.0

- Others
