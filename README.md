# Keyword Protocol 2000
The [KWP2000](https://en.wikipedia.org/wiki/Keyword_Protocol_2000/) is a communications protocol used for [on-board vehicle diagnostics systems (OBD)](https://en.wikipedia.org/wiki/On-board_diagnostics) by Suzuki (SDS), Kawasaki (KDS), Yamaha (YDS), Honda (HDS) and more. It is standardized by the [ISO 14230](https://www.iso.org/obp/ui/#iso:std:iso:14230:-1:ed-2:v1:en) and it is compatible with the [ISO 9141](https://www.iso.org/obp/ui/#iso:std:iso:9141:-2:ed-1:v1:en). Both uses a single line called the K-line through it we send some Parameter IDs, knowns as PIDs, to the [Electronic Control Unit (ECU)](https://en.wikipedia.org/wiki/Electronic_control_unit). This way we have total control on the motorbike: we are able to ask for sensors data (such as rpm, gear, speed, temperatures, etc.), see error codes and clear them, upload/download and more.

If you want to read more about this protocol and OBD-II you can see the [protocol explained](protocol.md).

## Supported Hardware
Any microcontroller (MCU) that uses the Arduino framework could be used.

The K-line works at 12V so you need an integrated circuit (IC) as interface, for example:
- L9637
- MC33660
- MC33199

Or, you can also use operational amplifiers/level shifter/transistors.

## List of Vehicles

The library would work on Suzuki and Kawasaki motorbikes. I didn't had the possibility to test on Yamaha and Honda bikes but I am willing to do, if you own any one of the two open an issue and I will write some code for it.

This is a list of motorcycles that have been successfully tested:

| Motorbike     | Year      | Tested |
| ------------- |---------- |------- |
| GSX-R600      | 2011      | ✔️    |
| Versys 650    | 2012      | ✔️    |

If your motorbike is not in this list open an issue and tell me if the library worked 😁

# Installation
Simply search for `KWP2000` in the Arduino/PlatformIO Library Manager or download this repository and add it to your library folder


# Usage
### Hardware
You have to find the K-line of your bike. Usually it's under the rider's seat. You just need to connect the K-line, VCC and GND to the driver IC you bought

Example of the wiring for the L9636:

![Alt text](extras/Images/l9637_wiring.png?raw=true "L9637 wiring")


##### Dealer Mode - Only for Suzuki:
If you wish also to be able to enter easily into this mode add an optocoupler with a ~330omh resistor between the MCU and the dealer pin

![Alt text](extras/Images/dealer_mode.png?raw=true "dealer mode")


### Software
Go to [PIDs.h](src/PIDs.h) and de-comment (delete the `//` symbols) your motorbike, then upload the [basic_working](examples/basic_working/basic_working.ino) example.






### Development
I made a [ECU Emulator](extras/ECU_Emulator) written in python for the development of new functions and tests.


### Documentation
Generally the functions return `true` if everything went fine, a `negative number` if there where any error, `false` if nothing changed

This documentation has been automatically generated with doxygen + doxylite, an automatic documentation generator, I will make the formatting nicer later.

See it here [documentation](documentation.md)


# Disclaimer
I do not assume and hereby disclaim any liability to any party for any damage to propriety or person


# Influence:
- ECU Hacking forum:
    - https://ecuhacking.activeboard.com/t56234221/kds-protocol
    - https://ecuhacking.activeboard.com/t22573776/sds-protocol/

- Arduino forum:
    - https://forum.arduino.cc/index.php?topic=236092.0
    - https://forum.arduino.cc/index.php?topic=334778.0

- Others
