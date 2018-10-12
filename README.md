# Keyword Protocol 2000
The [KWP2000](https://en.wikipedia.org/wiki/Keyword_Protocol_2000/) is a communications protocol used for on-board vehicle diagnostics systems (OBD) by Suzuki (SDS), Kawasaki (KDS), Yamaha (YDS), Honda (HDS) and more. It is based on the [ISO14230](https://www.iso.org/obp/ui/#iso:std:iso:14230:-1:ed-2:v1:en) and [ISO9141](https://www.iso.org/obp/ui/#iso:std:iso:9141:ed-1:v1:en) and uses a single line called the K-line. Using this communication protocol we send some Parameter IDs, knowns as [PID](https://en.wikipedia.org/wiki/OBD-II_PIDs).

More info in the links above, and in the extras folder

### Hardware
Any microcontroller (MCU) could be used. Many IC could be used as interface, for example:
- L9637
- MC33660
- MC33199
- operational amplifiers/level shifter (**not recconmended**)

### Tested Vehicles
- Suzuki GSX-R 600 L1 (2011)

# Usage
See the examples sketches, I also made a ECU Emulator written in python for the development of new functions/tests

## Installation
Simply add this from the Arduino Library Manager or download this repository and add it to your library folder

## Function Description
Generally the functions return `true` if everything went correct, a `negative number` if there where any error, `false` if nothing changed

## KWP2000 class
#### KWP2000(HardwareSerial *kline_serial, const uint8_t k_out_pin, const uint32_t kline_baudrate = 10400)
**Constructor:** choose the serial port which will be used to talk with the ECU and the TX pin of this serial

*Optional:* baudrate of the kline

Example:
> KWP2000 ECU (Serial2, 10);



### Setup
#### enableDebug(HardwareSerial *debug_serial, const uint32_t debug_baudrate = 115200, const uint8_t debug_level = DEBUG_LEVEL_DEFAULT)
**Choose and enable the debug:** select the serial port which will be used to print useful messages

*Optional:* baudrate, verbosity

Example:
> ECU.enableDebug(Serial);


#### setDebugLevel(const uint8_t debug_level)
**Change the verbosity of the debug messages:** you could choose between:
- DEBUG_LEVEL_NONE      no message will be print except the ones from the print functions
- DEBUG_LEVEL_DEFAULT   the most useful messages
- DEBUG_LEVEL_VERBOSE   print almost everythings happends between your MCU and the ECU

Example:
> ECU.setDebugLevel(DEBUG_LEVEL_VERBOSE);


#### disableDebug()
**Kill the debug messages**

Example:
> ECU.disableDebug();


#### enableDealerMode(const uint8_t dealer_pin)
**Enable the [dealer mode](https://www.gixxer.com/forums/80-06-07-gsx-r600-750/174828-gsxr-750-k6-k7-dealer-mode-bypass-guide.html):** Choose the pin which trigger the dealer mode *needs an optocoupler*

Example:
> ECU.enableDealerMode(14);


#### dealerMode(const uint8_t dealer_mode)
**Enable/disable the dealer mode**

Example:
> ECU.dealerMode(true);



### Comunation - Basic
#### initKline()
**Initialize the comunication with the ECU:** returns `false` until the inizialization sequence is not completed, `true` when it is completed and correct, a `negative number` if something has gone wrong. A private array would be create from this function to store the response from the ECU

*Optional:* select a different pointer which will contain the responses. See [*advanced_working.ino*](/examples/advanced_working/advanced_working.ino)

Example:
> while(ECU.initKline() == 0){;};


#### stopKline()
**Close the comunication with the ECU:** returns `false` until the end sequence is not completed, `true` when it is completed

*Optional:* if selected above a different memory address this would be deallocated

Example:
> while(ECU.stopKline() == 0){;};


#### keepAlive(uint16_t time = 1000)
**Keep the KLine active:** after 2 seconds of inactivity the KLine closes automatically, this function should be at the end of you *loop()* or anyway called often

*Optional:* choose how often this function will be called

Example:
> ECU.keepAlive();


#### requestSensorsData()
**Send a request and listen it's response:** this function does everything you need

Example:
> ECU.requestSensorsData();



### Comunation - Advanced
#### sendRequest(const uint8_t to_send[], const uint8_t send_len, const uint8_t wait_to_send_all = false, const uint8_t use_delay = false)
**Send a request to the ECU - Use this only if you know what are you doing:** select the *byte/uint8_t* array which will be sent and the size of this array

*Calling this without inizializing the kline could bring to unpredictable crashes*

*Optional:* wait until we empty the TX buffer (this takes about 15 microseconds fore a 8 byte request) and wait before making the next request

Example:
> ECU.sendRequest(start_sequence, sizeof(start_sequence));


#### listenResponse(uint8_t *resp = nullptr, uint8_t *resp_len = nullptr, const uint8_t use_delay = false)
**Listen the response from the ECU - Use this only if you know what are you doing:** this function fill an array with the ECU response, meanwhile it makes sure that the data are correct and for us

*Calling this without inizializing the kline could bring to unpredictable crashes*

*Optional:* change the memory addres where you want the response to be stored, wait before making the next request

Example:
> ECU.listenResponse();



### Print and Get Methods
#### printStatus(uint16_t time = 2000)
**Print some useful info and error messages:** 
```
---- STATUS ----
Connection:		Connected
Errors:			No
last data:		0.37 seconds ago
Baudrate:		115200
K-line TX pin:	22
Dealer pin:		25
Dealer mode:	Disabled

---- ERRORS ----
no errors
---- ------- ----
```
*Optional:* choose how often this function will be called

Example:
> ECU.printStatus();


#### printSensorsData(uint16_t time = 1)
**Print the sensors data:** needs to be called after *requestSensorsData()*

*Optional:* choose how often this function will be called

Example:
> ECU.printSensorsData();


#### printLastResponse()
**Print the last response received by the ECU**

Example:
> ECU.printLastResponse();


#### getStatus()
**Get the connection status of the ECU:** returns `true` if connected, `false` if not

Example:
> ECU.getStatus();


#### getError()
**Get a general error:** this function will just return `true` if there is no error, `-1` if there is any error. The library handles 12+ possible error codes, see them in *printStatus()*

Example:
> ECU.getError();


#### resetError()
**Remove all error codes**

Example:
> ECU.resetError();


#### get*
**get any sensor value**
Return the value of the sensor, actually it's possibile with:
- GPS: Gear Position Sensor
- RPM: Right Per Minutes
- Speed: speed of the bike
- TPS: Throttle Position Sensor
- IAP: Intake Air Pressure
- IAT: Intake Air Temperature
- ECT: Engine Coolant Temperature
- STPS: Secondary Throttle Position Sensor
- *more are coming*

Example:
> ECU.getRPM();



## Future Development
- test on other bikes
- discover new PIDs
- add more sensors

## Disclaimer
I do not assume and hereby disclaim any liability to any party for any damage to proprierty or person

## Version - Change Log
##### 1.0.0 - next
- road test passed :D
- version promoted for pubblication on the arduino library manager

##### 0.4.1 - Oct 12, 2018
- made two examples sketches
- added ECU_Emulator written in python
- merged printStatus and printError
- added printSensorData
- added printLastResponse
- added new errors and better error handling
- added documentation
- review all the code
- added more resources

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
