# ISO 14230
Here there are some useful resources if someone would like to hack on their own


## History
Unfortunately before the born of [International Organization for Standardization (ISO)](https://en.wikipedia.org/wiki/International_Organization_for_Standardization) and [Open Systems Interconnection (OSI)](https://en.wikipedia.org/wiki/OSI_model) there wasn't one single protocol for all the vehicles, instead almost every manufacter made their own. This resulted in a mess of different protocols, finally the need of a standard brought to the creation of the [OBD-II](https://en.wikipedia.org/wiki/On-board_diagnostics#OBD-II) which could comunicate with 5 different protocols (SAE J1850 PWM, SAE J1850 VPW, ISO 9141-2, ISO 14230 or KWP2000 and ISO 15765 CAN).

![Alt text](/extras/images/start.png?raw=true "Title")


## 1 - Start the comunication
This means we understood the structure of the request/response

![structure](/extras/images/structure.png)
Format: ![Alt Text](url)

and we are using the correct timing

![timing](/extras/images/timing.png)
Format: ![Alt Text](url)


## 2 - Receive the keybytes...
... and understand them
![Alt text](/extras/images/keybytes.png?raw=true "Title")


## 3 - Timing parameters
We need to respect them


## 4 - whatever
Now we can ask for sensors data, see error codes, upload/download from the ECU, and so on