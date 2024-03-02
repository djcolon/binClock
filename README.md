# binClock

**Daniel Colon - 19/02/2024**

A binary clock using an ESP8266 board with some 74HC595 shift registers to
drive LEDs.
Uses NTP to get time over the network.
Also has modes to show it's IP and ping-pong a led back and forth.

Built with an ESP32-C3 Mini, which I bought for 1.83
[here](https://www.aliexpress.com/item/1005005967641936.html)

[Built using PlatformIO](https://platformio.org/)

## Board

The PCB and schematic do not include the LEDs. An 8-pin connector is offered,
as well as a mounting hole for the LED ground. The intention is to use a piece
of 2mm solid core copper to attach the LED ground to, with wires leading to an
8 pin connector which will plug into the PCB. The entire board can be powered
via the ESP32's USB-C connector.

## Images

The prototype:

![The prototype](prototype.jpg?raw=true "The prototype")

The schematic:

![The schematic](schematic.png?raw=true "The schematic")

A preview of the PCB front:

![The PCB preview](pcb.png?raw=true "The PCB preview")

