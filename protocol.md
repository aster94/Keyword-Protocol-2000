# ISO 14230
Here there are some useful resources if someone would like to hack on their own. All I know cames from a document I found on google, I am not sure if I can put this document here (due to license) but you can find it online


## History
Unfortunately before the born of [International Organization for Standardization (ISO)](https://en.wikipedia.org/wiki/International_Organization_for_Standardization) and [Open Systems Interconnection (OSI)](https://en.wikipedia.org/wiki/OSI_model) there wasn't one single protocol for all the vehicles, instead almost every manufacter made their own. This resulted in a mess of different protocols, finally the need of a standard brought to the creation of the [OBD-II](https://en.wikipedia.org/wiki/On-board_diagnostics#OBD-II) which could comunicate with 5 different protocols (SAE J1850 PWM, SAE J1850 VPW, ISO 9141-2, ISO 14230 or KWP2000 and ISO 15765 CAN).

![start](/extras/Images/start.png)


## 1 - Start the comunication
This means we understood the structure of the request/response

![structure](/extras/Images/structure.png)

and we are using the correct timing

![timing](/extras/Images/timing.png)


## 2 - Receive the keybytes...
... and understand them
![key bytes](/extras/Images/keybytes.png)


## 3 - Timing parameters
We need to respect them


## 4 - whatever
Now we can ask for sensors data, see error codes, upload/download from the ECU, and so on