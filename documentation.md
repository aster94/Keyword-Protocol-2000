# Documentation
### [KWP2000](src/KWP2000.cpp#L80)
Description: Constructor for the
Parameters
`kline_serial`: The Serial port you will use to communicate with the ECU
`k_out_pin`: The TX pin of this serial
`kline_baudrate`: Optional, defaut to `10400`. The baudrate for the kline,
### [enableDebug](src/KWP2000.cpp#L96)
Description: Enable the debug of the communication.
Parameters
`debug_serial`: The Serial port you will use for the debug information
`debug_level`: Optional, default to `DEBUG_LEVEL_DEFAULT`. The verbosity of the debug
`debug_baudrate`: Optional, default to `115200`. The baudrate for the debug
### [setDebugLevel](src/KWP2000.cpp#L114)
Description: Change the debug level.
Parameters
`debug_level`: choose between DEBUG_LEVEL_NONE DEBUG_LEVEL_DEFAULT DEBUG_LEVEL_VERBOSE
### [disableDebug](src/KWP2000.cpp#L135)
Description: Disable the debug.
### [enableDealerMode](src/KWP2000.cpp#L150)
Description: Only for Suzuki: Enable the Dealer Mode.
Parameters
`dealer_pin`: The pin you will use to control it
### [dealerMode](src/KWP2000.cpp#L162)
Description: Only for Suzuki: Enable/Disable the Dealer Mode.
Parameters
`dealer_mode`: Choose between true/false
### [use_imperial](src/KWP2000.cpp#L173)
Description:
### [use_metric](src/KWP2000.cpp#L177)
Description:
### [initKline](src/KWP2000.cpp#L189)
Description: Inizialize the the communication through the K-Line.
### [stopKline](src/KWP2000.cpp#L335)
Description: Close the communication with the motorbike.
### [requestSensorsData](src/KWP2000.cpp#L410)
Description: Send a request to the ECU asking for data from all the sensors, to see them you can use `
### [readTroubleCodes](src/KWP2000.cpp#L515)
Description: Read the Diagnostic Trouble Codes (DTC) from the ECU.
Parameters
`which`: Optional, default to `READ_ONLY_ACTIVE`. One of the values from the `trouble_codes` enum
### [clearTroubleCodes](src/KWP2000.cpp#L549)
Description: Clear the DTC from the ECU.
Parameters
`code`: Optional. Only the passed `code` will be cleared
### [keepAlive](src/KWP2000.cpp#L567)
Description: Keep the connection through the K-Line alive.
Parameters
`time`: Optional. It is calculated automatically to be a safe interval
### [handleRequest](src/KWP2000.cpp#L656)
Description: This function is the core of the library. You just need to give a PID and it will generate the header, calculate the checksum and try to send the request. Then it will check if the response is correct and if now it will try to send the request another two times, all is based on the ISO14230.
Parameters
`to_send`: The PID you want to send, see PID.h for more detail
`send_len`: The lenght of the PID (use `sizeof` to get it)
`try_once`: Optional, default to `false`. Choose if you want to try to send the request 3 times in case of error
### [checkTimingParameter](src/KWP2000.cpp#L703)
Description:
### [accessTimingParameter](src/KWP2000.cpp#L745)
Description: Ask and print the Timing Parameters from the ECU.
Parameters
`read_only`: Optional, default to `true`. This avoid the possibility to unintentionally change them
### [resetTimingParameter](src/KWP2000.cpp#L821)
Description: Reset the Timing Parameters to the default settings from the ECU.
### [changeTimingParameter](src/KWP2000.cpp#L850)
Description: Change the Timing Parameters to custom ones.
Parameters
`new_atp`: Array of 5 elements containing the new parameters
`new_atp_len`: The lenght of the array (use `sizeof` to get it)
### [printStatus](src/KWP2000.cpp#L978)
Description: Print a rich and useful set of information about the ECU status and errors.
Parameters
`time`: Optional, default to `2000`milliseconds. The time between one print and the other
### [printSensorsData](src/KWP2000.cpp#L1099)
Description: Print all the sensors data from the ECU, you need to run `
### [printLastResponse](src/KWP2000.cpp#L1155)
Description: Print the last response received from the ECU.
### [getStatus](src/KWP2000.cpp#L1176)
Description: Get the connection status.
### [getError](src/KWP2000.cpp#L1186)
Description: This say you only if there are/aren't errors, to see them use `
### [resetError](src/KWP2000.cpp#L1201)
Description: Reset the errors from the ECU, use with caution.
### [getGPS](src/KWP2000.cpp#L1218)
Description: Get* the ECU sensor value you need GPS: Gear Position Sensor RPM: Right Per Minutes SPEED: speed of the bike TPS: Throttle Position Sensor IAP: Intake Air Pressure IAT: Intake Air Temperature ECT: Engine Coolant Temperature STPS: Secondary Throttle Position Sensor.
### [getRPM](src/KWP2000.cpp#L1223)
Description:
### [getSPEED](src/KWP2000.cpp#L1228)
Description:
### [getTPS](src/KWP2000.cpp#L1237)
Description:
### [getIAP](src/KWP2000.cpp#L1242)
Description:
### [getIAT](src/KWP2000.cpp#L1247)
Description:
### [getECT](src/KWP2000.cpp#L1256)
Description:
### [getSTPS](src/KWP2000.cpp#L1265)
Description:



Documentation generated with [Doxylite](https://github.com/aster94/Utilities)