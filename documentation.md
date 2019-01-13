# Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`define `[`maybe`](#_k_w_p2000_8cpp_1ac09cafa5804458ef54fd27acfe9ff78a)            | used when we don't know yet the behaviour of the K-Line
`define `[`TO_FAHRENHEIT`](#_k_w_p2000_8cpp_1a91e95ca0c2d1c25cd7bab761ef9331d0)            | the formula for the conversion
`define `[`LEN`](#_k_w_p2000_8cpp_1a3e4545636cef77da4ddab18e3d233101)            | complex but safe macro for the lenght
`define `[`ISO_MAX_DATA`](#_k_w_p2000_8cpp_1ab80bc64bf00fe64a0471b203280f3054)            | maximum lenght of a response from the ecu: 255 data + 4 header + 1 checksum
`define `[`ISO_T_P1`](#_k_w_p2000_8cpp_1a67399a7a07871f926d079819a57990a6)            | inter byte time for ECU response - min: 0 max: 20
`define `[`ISO_T_P2_MIN_LIMIT`](#_k_w_p2000_8cpp_1ae4b897217d8a7cb71d4e5e8de2d88470)            | 
`define `[`ISO_T_P2_MAX_LIMIT`](#_k_w_p2000_8cpp_1aae59d60e1e6bf444727c996601afdfe0)            | P2 time between tester request and ECU response or two ECU responses.
`define `[`ISO_T_P3_MAX_LIMIT`](#_k_w_p2000_8cpp_1a8720dab33fbb5c524752090fab43bc3c)            | P3 time between end of ECU responses and start of new tester request.
`define `[`ISO_T_P4_MAX_LIMIT`](#_k_w_p2000_8cpp_1aaa520653e7d97461654cf65413a1e402)            | inter byte time for tester request
`define `[`ISO_T_IDLE_NEW`](#_k_w_p2000_8cpp_1aaaf57595b6c6f1fa84605254930e8ad2)            | min 300, max undefinied
`define `[`ISO_T_INIL`](#_k_w_p2000_8cpp_1af51a5a6e779aeca1e37e990663601a84)            | Initialization low time.
`define `[`ISO_T_WUP`](#_k_w_p2000_8cpp_1a5ed4b5035e009d1d074a38128c025e1c)            | Wake up Pattern.
`define `[`request_ok`](#_p_i_ds_8h_1a78ce7705e54cbaa84ea562d4049227d2)            | 
`enum `[`error_enum`](#_k_w_p2000_8cpp_1ae7c16c55057fcaccff98f9725ba4d4ab)            | This is a a collection of possible ECU Errors.
`enum `[`debug_enum`](#_k_w_p2000_8h_1ad078f8d8e69df174a8cac511c1c069ab)            | Collection of possible debug levels.
`enum `[`trouble_codes`](#_k_w_p2000_8h_1ae4ad9c20cc20b9cee99316ffe2a90f95)            | Used by `readTroubleCodes()`
`class `[`KWP2000`](#class_k_w_p2000) | 

## Members

#### `define `[`maybe`](#_k_w_p2000_8cpp_1ac09cafa5804458ef54fd27acfe9ff78a) 

used when we don't know yet the behaviour of the K-Line

#### `define `[`TO_FAHRENHEIT`](#_k_w_p2000_8cpp_1a91e95ca0c2d1c25cd7bab761ef9331d0) 

the formula for the conversion

#### `define `[`LEN`](#_k_w_p2000_8cpp_1a3e4545636cef77da4ddab18e3d233101) 

complex but safe macro for the lenght

#### `define `[`ISO_MAX_DATA`](#_k_w_p2000_8cpp_1ab80bc64bf00fe64a0471b203280f3054) 

maximum lenght of a response from the ecu: 255 data + 4 header + 1 checksum

#### `define `[`ISO_T_P1`](#_k_w_p2000_8cpp_1a67399a7a07871f926d079819a57990a6) 

inter byte time for ECU response - min: 0 max: 20

#### `define `[`ISO_T_P2_MIN_LIMIT`](#_k_w_p2000_8cpp_1ae4b897217d8a7cb71d4e5e8de2d88470) 

#### `define `[`ISO_T_P2_MAX_LIMIT`](#_k_w_p2000_8cpp_1aae59d60e1e6bf444727c996601afdfe0) 

P2 time between tester request and ECU response or two ECU responses.

#### `define `[`ISO_T_P3_MAX_LIMIT`](#_k_w_p2000_8cpp_1a8720dab33fbb5c524752090fab43bc3c) 

P3 time between end of ECU responses and start of new tester request.

#### `define `[`ISO_T_P4_MAX_LIMIT`](#_k_w_p2000_8cpp_1aaa520653e7d97461654cf65413a1e402) 

inter byte time for tester request

#### `define `[`ISO_T_IDLE_NEW`](#_k_w_p2000_8cpp_1aaaf57595b6c6f1fa84605254930e8ad2) 

min 300, max undefinied

#### `define `[`ISO_T_INIL`](#_k_w_p2000_8cpp_1af51a5a6e779aeca1e37e990663601a84) 

Initialization low time.

#### `define `[`ISO_T_WUP`](#_k_w_p2000_8cpp_1a5ed4b5035e009d1d074a38128c025e1c) 

Wake up Pattern.

#### `define `[`request_ok`](#_p_i_ds_8h_1a78ce7705e54cbaa84ea562d4049227d2) 

#### `enum `[`error_enum`](#_k_w_p2000_8cpp_1ae7c16c55057fcaccff98f9725ba4d4ab) 

This is a a collection of possible ECU Errors.

 Values                         | Descriptions                                
--------------------------------|---------------------------------------------
EE_TEST            | for various test
EE_START            | unable to start comunication
EE_STOP            | unable to stop comunication
EE_TO            | data is not for us
EE_FROM            | data don't came from the ECU
EE_CS            | checksum error
EE_ECHO            | echo error
EE_UNEX            | unexpected error
EE_HEADER            | strange header
EE_USER            | error due to wrong call of a function
EE_CONFIG            | strange config value in the key bytes
EE_P3MAX            | time out communication
EE_CR            | check response error
EE_ATP            | problem setting the timing parameter
EE_WR            | We get a reject for a request we didn't sent.
EE_US            | not supported, yet
EE_TOTAL            | this is just to know how many possible errors are in this enum

#### `enum `[`debug_enum`](#_k_w_p2000_8h_1ad078f8d8e69df174a8cac511c1c069ab) 

Collection of possible debug levels.

 Values                         | Descriptions                                
--------------------------------|---------------------------------------------
DEBUG_LEVEL_NONE            | 
DEBUG_LEVEL_DEFAULT            | 
DEBUG_LEVEL_VERBOSE            | 

#### `enum `[`trouble_codes`](#_k_w_p2000_8h_1ae4ad9c20cc20b9cee99316ffe2a90f95) 

Used by `readTroubleCodes()`

 Values                         | Descriptions                                
--------------------------------|---------------------------------------------
READ_TOTAL            | 
READ_ONLY_ACTIVE            | 
READ_ALL            | 

# class `KWP2000` 

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  `[`KWP2000`](#class_k_w_p2000_1a025072150ac0b5a12c89b48fa349115e)`(HardwareSerial * kline_serial,const uint8_t k_out_pin,const uint32_t kline_baudrate)` | Constructor for the [KWP2000](#class_k_w_p2000) class.
`public void `[`enableDebug`](#class_k_w_p2000_1a35cce1caf07fbdc9338ef8a68f87f0d4)`(HardwareSerial * debug_serial,const uint8_t debug_level,const uint32_t debug_baudrate)` | Enable the debug of the communication.
`public void `[`setDebugLevel`](#class_k_w_p2000_1a606cca606a1dbf4bf10f4d5ae858c51c)`(const uint8_t debug_level)` | Change the debug level.
`public void `[`disableDebug`](#class_k_w_p2000_1a5e260665c48d5496b8f36a4366da801c)`()` | Disable the debug.
`public void `[`enableDealerMode`](#class_k_w_p2000_1abe3d7b98a45212a2a01136c435ec86cd)`(const uint8_t dealer_pin)` | Only for Suzuki: Enable the Dealer Mode.
`public void `[`dealerMode`](#class_k_w_p2000_1a1286399e7b04b99c4d52255becd49e54)`(const uint8_t dealer_mode)` | Only for Suzuki: Enable/Disable the Dealer Mode.
`public int8_t `[`initKline`](#class_k_w_p2000_1a525959756a83655e27c2f857aee22c7b)`()` | Inizialize the the communication through the K-Line.
`public int8_t `[`stopKline`](#class_k_w_p2000_1afe923cc38efb6d547d496ce8902f3d29)`()` | Close the communication with the motorbike.
`public void `[`requestSensorsData`](#class_k_w_p2000_1a0e0ce92a0eeafd35ccf4ac3655725d0c)`()` | Send a request to the ECU asking for data from all the sensors, to see them you can use `[printSensorsData()](#class_k_w_p2000_1ab9135d21a343e0e90f0527f4ab22b531)`
`public void `[`readTroubleCodes`](#class_k_w_p2000_1ad734605d8fcfb79cb04efb8595203b3a)`(const uint8_t which)` | Read the Diagnostic Trouble Codes (DTC) from the ECU.
`public void `[`clearTroubleCodes`](#class_k_w_p2000_1a311058a52c716a44efe8e32843467162)`(const uint8_t code)` | Clear the DTC from the ECU.
`public void `[`keepAlive`](#class_k_w_p2000_1aa3f5ac744841d4207190444b30436023)`(uint16_t time)` | Keep the connection through the K-Line alive.
`public int8_t `[`handleRequest`](#class_k_w_p2000_1ad11d8664b91f417a94528a8f92dd8d4d)`(const uint8_t to_send,const uint8_t send_len,const uint8_t try_once)` | This function is the core of the library. You just need to give a PID and it will generate the header, calculate the checksum and try to send the request. Then it will check if the response is correct and if now it will try to send the request another two times, all is based on the ISO14230.
`public void `[`accessTimingParameter`](#class_k_w_p2000_1a953906eda48ecaea0d1ad8a2f1b0e8d5)`(const uint8_t read_only)` | Ask and print the Timing Parameters from the ECU.
`public void `[`resetTimingParameter`](#class_k_w_p2000_1ab9246be67a37784274c444801878edb3)`()` | Reset the Timing Parameters to the default settings from the ECU.
`public void `[`changeTimingParameter`](#class_k_w_p2000_1a3ccc4ebcea810e12b2ceb63c9a4858a0)`(uint32_t new_atp,const uint8_t new_atp_len)` | Change the Timing Parameters to custom ones.
`public void `[`printStatus`](#class_k_w_p2000_1afefd32e48fc101c9b2307aa9a2c498b2)`(uint16_t time)` | Print a rich and useful set of information about the ECU status and errors.
`public void `[`printSensorsData`](#class_k_w_p2000_1ab9135d21a343e0e90f0527f4ab22b531)`()` | Print all the sensors data from the ECU, you need to run `[requestSensorsData()](#class_k_w_p2000_1a0e0ce92a0eeafd35ccf4ac3655725d0c)` before.
`public void `[`printLastResponse`](#class_k_w_p2000_1aead234d1c8fd1d0fa0ed0ff66ce478bf)`()` | Print the last response received from the ECU.
`public int8_t `[`getStatus`](#class_k_w_p2000_1ae52be2a2e5b21646da2bfcdc595b15f4)`()` | Get the connection status.
`public int8_t `[`getError`](#class_k_w_p2000_1a2b1b535611064e42990ebfe292c3f53a)`()` | This say you only if there are/aren't errors, to see them use `[printStatus()](#class_k_w_p2000_1afefd32e48fc101c9b2307aa9a2c498b2)`
`public void `[`resetError`](#class_k_w_p2000_1a44fbde61c33c7cbe7b7abc881735c595)`()` | Reset the errors from the ECU, use with caution.
`public uint8_t `[`getGPS`](#class_k_w_p2000_1a8981e2ea9bc9f222518e913965146b7e)`()` | Get* the ECU sensor value you need GPS: Gear Position Sensor RPM: Right Per Minutes SPEED: speed of the bike TPS: Throttle Position Sensor IAP: Intake Air Pressure IAT: Intake Air Temperature ECT: Engine Coolant Temperature STPS: Secondary Throttle Position Sensor.
`public uint8_t `[`getRPM`](#class_k_w_p2000_1a89113f189900e5d00a34ef48b144a3e4)`()` | 
`public uint8_t `[`getSPEED`](#class_k_w_p2000_1a456e9b164d6eaa2ed3eea97ded4cb7a5)`()` | 
`public uint8_t `[`getTPS`](#class_k_w_p2000_1aeb492557cd6d2fc085201b9c831c1051)`()` | 
`public uint8_t `[`getIAP`](#class_k_w_p2000_1a86430bdb898cb2540cca22e05d2d726b)`()` | 
`public uint8_t `[`getIAT`](#class_k_w_p2000_1aad217aa63b67e50060a5268305b276c8)`()` | 
`public uint8_t `[`getECT`](#class_k_w_p2000_1a34a97926feddf5616960946ee09af80e)`()` | 
`public uint8_t `[`getSTPS`](#class_k_w_p2000_1a734b4f7292f29c903469aa572d1fa4d1)`()` | 

## Members

#### `public  `[`KWP2000`](#class_k_w_p2000_1a025072150ac0b5a12c89b48fa349115e)`(HardwareSerial * kline_serial,const uint8_t k_out_pin,const uint32_t kline_baudrate)` 

Constructor for the [KWP2000](#class_k_w_p2000) class.

#### Parameters
* `kline_serial` The Serial port you will use to communicate with the ECU 

* `k_out_pin` The TX pin of this serial 

* `kline_baudrate` Optional, defaut to `10400`. The baudrate for the kline,

#### `public void `[`enableDebug`](#class_k_w_p2000_1a35cce1caf07fbdc9338ef8a68f87f0d4)`(HardwareSerial * debug_serial,const uint8_t debug_level,const uint32_t debug_baudrate)` 

Enable the debug of the communication.

#### Parameters
* `debug_serial` The Serial port you will use for the debug information 

* `debug_level` Optional, default to `DEBUG_LEVEL_DEFAULT`. The verbosity of the debug 

* `debug_baudrate` Optional, default to `115200`. The baudrate for the debug

#### `public void `[`setDebugLevel`](#class_k_w_p2000_1a606cca606a1dbf4bf10f4d5ae858c51c)`(const uint8_t debug_level)` 

Change the debug level.

#### Parameters
* `debug_level` choose between DEBUG_LEVEL_NONE DEBUG_LEVEL_DEFAULT DEBUG_LEVEL_VERBOSE

#### `public void `[`disableDebug`](#class_k_w_p2000_1a5e260665c48d5496b8f36a4366da801c)`()` 

Disable the debug.

#### `public void `[`enableDealerMode`](#class_k_w_p2000_1abe3d7b98a45212a2a01136c435ec86cd)`(const uint8_t dealer_pin)` 

Only for Suzuki: Enable the Dealer Mode.

#### Parameters
* `dealer_pin` The pin you will use to control it

#### `public void `[`dealerMode`](#class_k_w_p2000_1a1286399e7b04b99c4d52255becd49e54)`(const uint8_t dealer_mode)` 

Only for Suzuki: Enable/Disable the Dealer Mode.

#### Parameters
* `dealer_mode` Choose between true/false

#### `public int8_t `[`initKline`](#class_k_w_p2000_1a525959756a83655e27c2f857aee22c7b)`()` 

Inizialize the the communication through the K-Line.

#### Returns
`0` until the connection is not established, then `true` if there aren't any errors, a `negative number` otherwise

#### `public int8_t `[`stopKline`](#class_k_w_p2000_1afe923cc38efb6d547d496ce8902f3d29)`()` 

Close the communication with the motorbike.

#### Returns
`0` until the connection is not closed, then `true` if there aren't any errors, a `negative number` otherwise

#### `public void `[`requestSensorsData`](#class_k_w_p2000_1a0e0ce92a0eeafd35ccf4ac3655725d0c)`()` 

Send a request to the ECU asking for data from all the sensors, to see them you can use `[printSensorsData()](#class_k_w_p2000_1ab9135d21a343e0e90f0527f4ab22b531)`

#### `public void `[`readTroubleCodes`](#class_k_w_p2000_1ad734605d8fcfb79cb04efb8595203b3a)`(const uint8_t which)` 

Read the Diagnostic Trouble Codes (DTC) from the ECU.

#### Parameters
* `which` Optional, default to `READ_ONLY_ACTIVE`. One of the values from the `trouble_codes` enum

#### `public void `[`clearTroubleCodes`](#class_k_w_p2000_1a311058a52c716a44efe8e32843467162)`(const uint8_t code)` 

Clear the DTC from the ECU.

#### Parameters
* `code` Optional. Only the passed `code` will be cleared

#### `public void `[`keepAlive`](#class_k_w_p2000_1aa3f5ac744841d4207190444b30436023)`(uint16_t time)` 

Keep the connection through the K-Line alive.

#### Parameters
* `time` Optional. It is calculated automatically to be a safe interval

#### `public int8_t `[`handleRequest`](#class_k_w_p2000_1ad11d8664b91f417a94528a8f92dd8d4d)`(const uint8_t to_send,const uint8_t send_len,const uint8_t try_once)` 

This function is the core of the library. You just need to give a PID and it will generate the header, calculate the checksum and try to send the request. Then it will check if the response is correct and if now it will try to send the request another two times, all is based on the ISO14230.

#### Parameters
* `to_send` The PID you want to send, see PID.h for more detail 

* `send_len` The lenght of the PID (use `sizeof` to get it) 

* `try_once` Optional, default to `false`. Choose if you want to try to send the request 3 times in case of error 

#### Returns
`true` if the request has been sent and a correct response has been received, a `negative number` otherwise

#### `public void `[`accessTimingParameter`](#class_k_w_p2000_1a953906eda48ecaea0d1ad8a2f1b0e8d5)`(const uint8_t read_only)` 

Ask and print the Timing Parameters from the ECU.

#### Parameters
* `read_only` Optional, default to `true`. This avoid the possibility to unintentionally change them

#### `public void `[`resetTimingParameter`](#class_k_w_p2000_1ab9246be67a37784274c444801878edb3)`()` 

Reset the Timing Parameters to the default settings from the ECU.

#### `public void `[`changeTimingParameter`](#class_k_w_p2000_1a3ccc4ebcea810e12b2ceb63c9a4858a0)`(uint32_t new_atp,const uint8_t new_atp_len)` 

Change the Timing Parameters to custom ones.

#### Parameters
* `new_atp` Array of 5 elements containing the new parameters 

* `new_atp_len` The lenght of the array (use `sizeof` to get it)

#### `public void `[`printStatus`](#class_k_w_p2000_1afefd32e48fc101c9b2307aa9a2c498b2)`(uint16_t time)` 

Print a rich and useful set of information about the ECU status and errors.

#### Parameters
* `time` Optional, default to `2000`milliseconds. The time between one print and the other

#### `public void `[`printSensorsData`](#class_k_w_p2000_1ab9135d21a343e0e90f0527f4ab22b531)`()` 

Print all the sensors data from the ECU, you need to run `[requestSensorsData()](#class_k_w_p2000_1a0e0ce92a0eeafd35ccf4ac3655725d0c)` before.

#### `public void `[`printLastResponse`](#class_k_w_p2000_1aead234d1c8fd1d0fa0ed0ff66ce478bf)`()` 

Print the last response received from the ECU.

#### `public int8_t `[`getStatus`](#class_k_w_p2000_1ae52be2a2e5b21646da2bfcdc595b15f4)`()` 

Get the connection status.

#### Returns
It could be `true` or `false`

#### `public int8_t `[`getError`](#class_k_w_p2000_1a2b1b535611064e42990ebfe292c3f53a)`()` 

This say you only if there are/aren't errors, to see them use `[printStatus()](#class_k_w_p2000_1afefd32e48fc101c9b2307aa9a2c498b2)`

#### Returns
It could be `true` or `false`

#### `public void `[`resetError`](#class_k_w_p2000_1a44fbde61c33c7cbe7b7abc881735c595)`()` 

Reset the errors from the ECU, use with caution.

#### `public uint8_t `[`getGPS`](#class_k_w_p2000_1a8981e2ea9bc9f222518e913965146b7e)`()` 

Get* the ECU sensor value you need GPS: Gear Position Sensor RPM: Right Per Minutes SPEED: speed of the bike TPS: Throttle Position Sensor IAP: Intake Air Pressure IAT: Intake Air Temperature ECT: Engine Coolant Temperature STPS: Secondary Throttle Position Sensor.

#### Returns
The sensor value from the ECU

#### `public uint8_t `[`getRPM`](#class_k_w_p2000_1a89113f189900e5d00a34ef48b144a3e4)`()` 

#### `public uint8_t `[`getSPEED`](#class_k_w_p2000_1a456e9b164d6eaa2ed3eea97ded4cb7a5)`()` 

#### `public uint8_t `[`getTPS`](#class_k_w_p2000_1aeb492557cd6d2fc085201b9c831c1051)`()` 

#### `public uint8_t `[`getIAP`](#class_k_w_p2000_1a86430bdb898cb2540cca22e05d2d726b)`()` 

#### `public uint8_t `[`getIAT`](#class_k_w_p2000_1aad217aa63b67e50060a5268305b276c8)`()` 

#### `public uint8_t `[`getECT`](#class_k_w_p2000_1a34a97926feddf5616960946ee09af80e)`()` 

#### `public uint8_t `[`getSTPS`](#class_k_w_p2000_1a734b4f7292f29c903469aa572d1fa4d1)`()` 

Generated by [Moxygen](https://sourcey.com/moxygen)