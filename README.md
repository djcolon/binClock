# binClock

**Daniel Colon - 19/02/2024**

A binary clock using an ESP8266 board with some 74HC595 shift registers to
drive LEDs.
Uses NTP to get time over the network.
Also has modes to show it's IP and ping-pong a led back and forth.

Built with an ESP32-C3 Mini, which I bought for 1.83
[here](https://www.aliexpress.com/item/1005005967641936.html)

![The prototype](prototype.jpg?raw=true "The prototype")

[Built using PlatformIO](https://platformio.org/)