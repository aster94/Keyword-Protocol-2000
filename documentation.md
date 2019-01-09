# class `KWP2000` 


## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  `[`KWP2000`](#class_k_w_p2000_1a025072150ac0b5a12c89b48fa349115e)`(HardwareSerial * kline_serial,const uint8_t k_out_pin,const uint32_t kline_baudrate)` | Constructor for the [KWP2000](#class_k_w_p2000) class.
`public void `[`enableDebug`](#class_k_w_p2000_1a35cce1caf07fbdc9338ef8a68f87f0d4)`(HardwareSerial * debug_serial,const uint8_t debug_level,const uint32_t debug_baudrate)` | Enable the debug of the communication.
`public void `[`setDebugLevel`](#class_k_w_p2000_1a606cca606a1dbf4bf10f4d5ae858c51c)`(const uint8_t debug_level)` | Change the debug level.
`public void `[`disableDebug`](#class_k_w_p2000_1a5e260665c48d5496b8f36a4366da801c)`()` | Disable the debug.
`public void `[`enableDealerMode`](#class_k_w_p2000_1abe3d7b98a45212a2a01136c435ec86cd)`(const uint8_t dealer_pin)` | Only for Suzuki: Enable the Dealer Mode.
`public void `[`dealerMode`](#class_k_w_p2000_1a1286399e7b04b99c4d52255becd49e54)`(const uint8_t dealer_mode)` | Enable/Disable the Dealer Mode.
`public int8_t `[`initKline`](#class_k_w_p2000_1a525959756a83655e27c2f857aee22c7b)`()` | Inizialize the the communication through the K-Line.
`public int8_t `[`stopKline`](#class_k_w_p2000_1afe923cc38efb6d547d496ce8902f3d29)`()` | Close the communication with the motorbike.
`public void `[`requestSensorsData`](#class_k_w_p2000_1a0e0ce92a0eeafd35ccf4ac3655725d0c)`()` | Send a request to the ECU asking for data from all the sensors, to see them you can use `[printSensorsData()](#class_k_w_p2000_1ab9135d21a343e0e90f0527f4ab22b531)`
`public void `[`keepAlive`](#class_k_w_p2000_1aa3f5ac744841d4207190444b30436023)`(uint16_t time)` | Keep the connection through the K-Line alive.
`public int8_t `[`handleRequest`](#class_k_w_p2000_1ab12d216d5ccab1d197c6847783ac2125)`(const uint8_t to_send,const uint8_t send_len)` | This function is part of the core of the library. You just need to give a PID and it will generate the header, calculate the checksum and try to send the request with error hadling options.
`public void `[`accessTimingParameter`](#class_k_w_p2000_1a953906eda48ecaea0d1ad8a2f1b0e8d5)`(const uint8_t read_only)` | Ask and print the Timing Parameters from the ECU.
`public void `[`resetTimingParameter`](#class_k_w_p2000_1ab9246be67a37784274c444801878edb3)`()` | Reset the Timing Parameters to the default settings from the ECU.
`public void `[`changeTimingParameter`](#class_k_w_p2000_1abd42cfde45d2f41846915b58dfe9c15e)`(uint32_t new_atp,const uint8_t new_atp_len)` | Change the Timing Parameters to custom ones.
`public void `[`printStatus`](#class_k_w_p2000_1afefd32e48fc101c9b2307aa9a2c498b2)`(uint16_t time)` | Print a rich and useful set of information about the ECU status and errors.
`public void `[`printSensorsData`](#class_k_w_p2000_1ab9135d21a343e0e90f0527f4ab22b531)`()` | Print all the sensors data from the ECU, you need to run `[requestSensorsData()](#class_k_w_p2000_1a0e0ce92a0eeafd35ccf4ac3655725d0c)` before.
`public void `[`printLastResponse`](#class_k_w_p2000_1aead234d1c8fd1d0fa0ed0ff66ce478bf)`()` | Print the last response received from the ECU.
`public int8_t `[`getStatus`](#class_k_w_p2000_1ae52be2a2e5b21646da2bfcdc595b15f4)`()` | Get the connection status.
`public int8_t `[`getError`](#class_k_w_p2000_1a2b1b535611064e42990ebfe292c3f53a)`()` | This say you only if there are/aren't errors, to see them use `[printStatus()](#class_k_w_p2000_1afefd32e48fc101c9b2307aa9a2c498b2)`
`public void `[`resetError`](#class_k_w_p2000_1a44fbde61c33c7cbe7b7abc881735c595)`()` | Reset the errors from the ECU, use with caution.
`public uint8_t `[`getGPS`](#class_k_w_p2000_1a8981e2ea9bc9f222518e913965146b7e)`()` | Get*.
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

* `kline_baudrate` The baudrate for the kline (defaut to 10400)

#### `public void `[`enableDebug`](#class_k_w_p2000_1a35cce1caf07fbdc9338ef8a68f87f0d4)`(HardwareSerial * debug_serial,const uint8_t debug_level,const uint32_t debug_baudrate)` 

Enable the debug of the communication.

#### Parameters
* `debug_serial` The Serial port you will use for the debug information 

* `debug_level` Default: `DEBUG_LEVEL_DEFAULT` The verbosity of the debug 

* `debug_baudrate` Default: `115200` The baudrate for the debug

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

Enable/Disable the Dealer Mode.

#### Parameters
* `dealer_mode` Choose between true/false

#### `public int8_t `[`initKline`](#class_k_w_p2000_1a525959756a83655e27c2f857aee22c7b)`()` 

Inizialize the the communication through the K-Line.

#### Returns
int8_t return `0` until the connection is not established, then `true` if there aren't any errors, a `negative number` otherwise

#### `public int8_t `[`stopKline`](#class_k_w_p2000_1afe923cc38efb6d547d496ce8902f3d29)`()` 

Close the communication with the motorbike.

#### Returns
int8_t return `0` until the connection is not closed, then `true` if there aren't any errors, a `negative number` otherwise

## `public void `[`requestSensorsData`](#class_k_w_p2000_1a0e0ce92a0eeafd35ccf4ac3655725d0c)`()` 

Send a request to the ECU asking for data from all the sensors, to see them you can use `[printSensorsData()](#class_k_w_p2000_1ab9135d21a343e0e90f0527f4ab22b531)`

## `public void `[`keepAlive`](#class_k_w_p2000_1aa3f5ac744841d4207190444b30436023)`(uint16_t time)` 

Keep the connection through the K-Line alive.

#### Parameters
* `time` It is calculated automatically to be a safe interval

## `public int8_t `[`handleRequest`](#class_k_w_p2000_1ab12d216d5ccab1d197c6847783ac2125)`(const uint8_t to_send,const uint8_t send_len)` 

This function is part of the core of the library. You just need to give a PID and it will generate the header, calculate the checksum and try to send the request with error hadling options.

#### Parameters
* `to_send` The PID you want to send, see PID.h for more detail 

* `send_len` The lenght of the PID (use `sizeof` to get it) 

#### Returns
int8_t return `true` if the request has been sent and a correct response has been received, a `negative number` otherwise

## `public void `[`accessTimingParameter`](#class_k_w_p2000_1a953906eda48ecaea0d1ad8a2f1b0e8d5)`(const uint8_t read_only)` 

Ask and print the Timing Parameters from the ECU.

#### Parameters
* `read_only` Default: `true` this avoid the possibility to unintentionally change them

## `public void `[`resetTimingParameter`](#class_k_w_p2000_1ab9246be67a37784274c444801878edb3)`()` 

Reset the Timing Parameters to the default settings from the ECU.

## `public void `[`changeTimingParameter`](#class_k_w_p2000_1abd42cfde45d2f41846915b58dfe9c15e)`(uint32_t new_atp,const uint8_t new_atp_len)` 

Change the Timing Parameters to custom ones.

#### Parameters
* `new_atp` Array of 5 elements containing the new parameters 

* `new_atp_len` The lenght of the array (use `sizeof` to get it)

## `public void `[`printStatus`](#class_k_w_p2000_1afefd32e48fc101c9b2307aa9a2c498b2)`(uint16_t time)` 

Print a rich and useful set of information about the ECU status and errors.

#### Parameters
* `time` Default: `2000ms` the time between one print and the other

## `public void `[`printSensorsData`](#class_k_w_p2000_1ab9135d21a343e0e90f0527f4ab22b531)`()` 

Print all the sensors data from the ECU, you need to run `[requestSensorsData()](#class_k_w_p2000_1a0e0ce92a0eeafd35ccf4ac3655725d0c)` before.

## `public void `[`printLastResponse`](#class_k_w_p2000_1aead234d1c8fd1d0fa0ed0ff66ce478bf)`()` 

Print the last response received from the ECU.

## `public int8_t `[`getStatus`](#class_k_w_p2000_1ae52be2a2e5b21646da2bfcdc595b15f4)`()` 

Get the connection status.

#### Returns
int8_t It could be `true` or `false`

## `public int8_t `[`getError`](#class_k_w_p2000_1a2b1b535611064e42990ebfe292c3f53a)`()` 

This say you only if there are/aren't errors, to see them use `[printStatus()](#class_k_w_p2000_1afefd32e48fc101c9b2307aa9a2c498b2)`

#### Returns
int8_t It could be `true` or `false`

## `public void `[`resetError`](#class_k_w_p2000_1a44fbde61c33c7cbe7b7abc881735c595)`()` 

Reset the errors from the ECU, use with caution.

## `public uint8_t `[`getGPS`](#class_k_w_p2000_1a8981e2ea9bc9f222518e913965146b7e)`()` 

Get*.

#### Returns
uint8_t Get the sensor value from the ECU

#### `public uint8_t `[`getRPM`](#class_k_w_p2000_1a89113f189900e5d00a34ef48b144a3e4)`()` 

#### `public uint8_t `[`getSPEED`](#class_k_w_p2000_1a456e9b164d6eaa2ed3eea97ded4cb7a5)`()` 

#### `public uint8_t `[`getTPS`](#class_k_w_p2000_1aeb492557cd6d2fc085201b9c831c1051)`()` 

#### `public uint8_t `[`getIAP`](#class_k_w_p2000_1a86430bdb898cb2540cca22e05d2d726b)`()` 

#### `public uint8_t `[`getIAT`](#class_k_w_p2000_1aad217aa63b67e50060a5268305b276c8)`()` 

#### `public uint8_t `[`getECT`](#class_k_w_p2000_1a34a97926feddf5616960946ee09af80e)`()` 

#### `public uint8_t `[`getSTPS`](#class_k_w_p2000_1a734b4f7292f29c903469aa572d1fa4d1)`()` 

Generated by [Moxygen](https://sourcey.com/moxygen)