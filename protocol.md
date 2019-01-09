# ISO 14230 - KWP2000
Here there are some useful resources if someone would like to hack on his own. All I know cames from a document I found on google, none of the above tables or graph were produced by me. I am not sure if I can put this document here (due to license and copyright) but you can find it online

## History
Unfortunately before the born of [International Organization for Standardization (ISO)](https://en.wikipedia.org/wiki/International_Organization_for_Standardization) and [Open Systems Interconnection (OSI)](https://en.wikipedia.org/wiki/OSI_model) there wasn't one single protocol for all the vehicles, instead almost every manufacter made their own. This resulted in a mess of different protocols, finally the need of a standard brought to the creation of the [OBD-II](https://en.wikipedia.org/wiki/On-board_diagnostics#OBD-II) which could comunicate with 5 different protocols:
- SAE J1850 PWM - old cars from General Motors
- SAE J1850 VPW - old cars from Ford Motor Company
- ISO 9141 - Chrysler and some European and Asian vehicles
- ISO 14230 or KWP2000 - used by most manufacturers, especially by Japanese motorcycles
- ISO 15765 CAN - newer protocol used by BMW

![Alt text](/extras/Images/obd2_connector.png?raw=true "obd2_connector")


# Comunication
Before we begin this is the structure of the packets we would send/receive from the ECU

![Alt text](/extras/Images/structure.png?raw=true "structure")


Also the timing between bytes, requests, responses is crucial

![Alt text](/extras/Images/timing.png?raw=true "timing")


## 1 - Inizialize
Finally we can start the connection

![Alt text](/extras/Images/fast_init.png?raw=true "fast init")


## 2 - Receive the keybytes...
We will receive a confirmation from the ECU and two more bytes which would say to us how the motorbike expect to receive the request from us

![Alt text](/extras/Images/keybytes.png?raw=true "key bytes")


## 3 - Timing parameters
Then, for stronger safety, we will ask to the ECU its limits about timing

![Alt text](/extras/Images/timing_parameter.png?raw=true "timing parameter")


## 4 - Enojoy
Now we can ask for sensors data, see error codes, upload/download from the ECU, and so on
