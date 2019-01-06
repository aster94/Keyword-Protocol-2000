# Keyword Protocol 2000
The [KWP2000](https://en.wikipedia.org/wiki/Keyword_Protocol_2000/) is a communications protocol used for [on-board vehicle diagnostics systems (OBD)](https://en.wikipedia.org/wiki/On-board_diagnostics) by Suzuki (SDS), Kawasaki (KDS), Yamaha (YDS), Honda (HDS) and more. It is standardized by the [ISO 14230](https://www.iso.org/obp/ui/#iso:std:iso:14230:-1:ed-2:v1:en) and it is compatible with the [ISO 9141-2](https://www.iso.org/obp/ui/#iso:std:iso:9141:-2:ed-1:v1:en). Both uses a single line called the K-line through it we send some Parameter IDs, knowns as PIDs, to the [Electronic Control Unit (ECU)](https://en.wikipedia.org/wiki/Electronic_control_unit). This way we have total control on the ECU: we are able to ask for sensors data (such as RPM, gear, speed, temperatures, etc.), see error codes (clear them), upload/download and so on

If you want to read more about this protocol and OBD-II you can see the [protocol explained](protocol.md)

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
You have to find the K-line of your bike. In suzuki bikes it's under the rider's seat. You just need to connect the Kline, Vcc and Gnd to the driver IC you bought

Example of the wiring for the L9636
![Alt text](/extras/Images/l9637_wiring.png?raw=true "L9637 wiring")


###### Dealer Mode - Only for Suzuki
If you wish also to be able to enter easily into this mode add an optocoupler with a ~330omh resistor between the MCU and the dealer pin
![Alt text](/extras/Images/dealer_mode.png?raw=true "dealer mode")

### Software
First of all go to [PIDs.h](/src/PIDs.h) and decomment (delete the `//` symbols) your motorbike, then upload any of the [examples](/examples/)
I also made a [ECU Emulator](/extras/ECU_Emulator) written in python for the development of new functions and tests

## Installation
Simply add this from the Arduino/PlatformIO Library Manager or download this repository and add it to your library folder

## Function Description
Generally the functions return `true` if everything went fine, a `negative number` if there where any error, `false` if nothing changed

# Documentation
doxygen?


## Disclaimer
I do not assume and hereby disclaim any liability to any party for any damage to proprierty or person


## Influence:
- [ECU Hacking forum](https://ecuhacking.activeboard.com/): 
    - https://ecuhacking.activeboard.com/t56234221/kds-protocol
    - https://ecuhacking.activeboard.com/t22573776/sds-protocol/

- [Arduino forum](https://forum.arduino.cc/)
    - https://forum.arduino.cc/index.php?topic=236092.0

- Others
