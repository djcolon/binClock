#include <Arduino.h>

// WiFi
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#define INTERNAL_LED D4
#define CLOCK D0
#define LATCH D1
#define DATA D2

union Registers {
  uint8_t register1;
  uint8_t register2;
  uint8_t register3;
  uint8_t register4;
};

Registers registers;

void updateRegisters() {
  digitalWrite(LATCH, LOW);
  shiftOut(DATA, CLOCK, MSBFIRST, registers.register1);
  shiftOut(DATA, CLOCK, MSBFIRST, registers.register2);
  shiftOut(DATA, CLOCK, MSBFIRST, registers.register3);
  shiftOut(DATA, CLOCK, MSBFIRST, registers.register4);
  digitalWrite(LATCH, HIGH);
}



void setup() {
  // Pins
  pinMode(INTERNAL_LED, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(LATCH, OUTPUT);
  // Serial
  Serial.begin(115200);
  // WiFi
  //Local initialization. Once its business is done, there is no need to keep
  // it around.
  WiFiManager wifiManager;
  //reset saved settings
  //wifiManager.resetSettings();
  
  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("binClock");
  //if you get here you have connected to the WiFi
  Serial.println("Connected to WiFi. Starting.");
}

void loop() {
  // Update seconds.
  registers.register1++;
  if(registers.register1 == 60) {
    registers.register1 = 0;
    // Seconds rolled over, increase minutes.
    registers.register2++;
  }

  // Update minutes.
  registers.register2++;
  // We may have had a double increase, this comparison and subtraction will
  // deal with that.
  if(registers.register2 >= 60) {
    registers.register2 = registers.register2 - 60;
    // Minutes rolled over, increase hours.
    registers.register3++;
  }

  // Update hours.
  registers.register3++;
  if(registers.register3 > 24) {
    registers.register3 = registers.register3 - 24;
  }
  
  updateRegisters();

  delay(1000);
}
