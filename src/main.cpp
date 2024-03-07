// Libraries ///////////////////////////////////////////////////////////////////
#include <Arduino.h>
// WiFi
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <mDNS.h>
// Buttons
#include <EasyButton.h>
// Time
#include "time.h"
// Modes
#include "modeManager.cpp"

// Includes ////////////////////////////////////////////////////////////////////
#include "registers.h"
#include "modes/binClock.cpp"
#include "modes/pingPong.cpp"
#include "modes/showIp.cpp"
#include "modes/allOn.cpp"

// Definitions /////////////////////////////////////////////////////////////////
// Pins
#define INTERNAL_LED 8
#define CLOCK 8
#define LATCH 7
#define DATA 6
#define OE 5

// Buttons
#define BUTTON1 0
#define BUTTON2 1

// Flags
#define WRITEMSBFIRST
//#define DEBUG

// Config
#define HOSTNAME "binClock"

// Globals /////////////////////////////////////////////////////////////////////
EasyButton button1(BUTTON1);
EasyButton button2(BUTTON2);

Registers registers;

ModeManager modeManager;

// Other globals
// Brightness is inverted!
uint8_t brightness = 0;

// Functions ///////////////////////////////////////////////////////////////////

// Reads button states.
bool getButtonState(int buttonNumber) {
  switch(buttonNumber) {
    case 1:
      return digitalRead(BUTTON1) == HIGH;
    case 2:
      return digitalRead(BUTTON2) == HIGH;
    default:
      return false;
  }
}

// Shifts out our register in one go.
// Arduino shiftOut may because of weird glitches?
// Data is set on rising edge of clock. Hold time is 4.5ns (datasheet)
// This gives a rate of 222MHz, given that the chips is clocked at 26MHz, the
// factor 10 means we won't have to worry about timing. Chip runs faster than
// our program (especially with Arduino libs in between)!
void shiftOut32() {
  digitalWrite(LATCH, LOW);
  bool bit;
  for (uint8_t i = 0; i < 32; i++)
  {
    #ifdef WRITEMSBFIRST
      bit = !!(registers.asInt & (1 << (31 - i)));
    #else
      bit = !!(registers.asInt & (1 << i));
    #endif
    digitalWrite(DATA, bit);
    #ifdef DEBUG
      Serial.print(bit);
    #endif
    digitalWrite(CLOCK, HIGH);
    digitalWrite(CLOCK, LOW);
  }
  digitalWrite(LATCH, HIGH);
  #ifdef DEBUG
    Serial.print('\n');
  #endif
}

// Cycle through modes.
void onButton1Press() {
  Serial.println("Switching modes.");
  Serial.printf("Old mode: %s.\n", modeManager.getActiveMode()->getFriendlyName());
  modeManager.nextMode(registers);
  Serial.printf("New mode: %s.\n", modeManager.getActiveMode()->getFriendlyName());
}

// Cycle brightness.
void onButton2Press() {
  // 0 is full brightness, 255 is off.
  if(brightness == 255) {
    brightness = 0;
  } else {
    brightness = brightness + 64;
  }
  #ifdef DEBUG
    Serial.printf("Setting brightness to: %d.\n", brightness);
  #endif
  analogWrite(OE, brightness);
}

/**
 * Sets up mDNS so we have a name on the network.
*/
void setupMdns() {
    //initialize mDNS service
    esp_err_t err = mdns_init();
    if (err) {
        printf("MDNS Init failed: %d\n", err);
        return;
    }

    //set hostname
    mdns_hostname_set(HOSTNAME);
    //set default instance
    mdns_instance_name_set(HOSTNAME);
}

/**
 * Initialises modes. Add custom modes here.
*/
void setupModeManager() {
  modeManager.registerMode(new BinClock());
  modeManager.registerMode(new PingPong());
  modeManager.registerMode(new ShowIp());
  modeManager.registerMode(new AllOn());
  modeManager.setup(registers);
  registers.asInt = 0b00100000;
  shiftOut32();
}

/**
 * Called on boot.
*/
void setup() {
  // Pins
  pinMode(INTERNAL_LED, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(LATCH, OUTPUT);
  pinMode(OE, OUTPUT);
  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
  digitalWrite(CLOCK, LOW);
  analogWrite(OE, brightness);
  // Set leds to init.
  registers.asInt = 0b00000001;
  shiftOut32();
  // Serial
  Serial.begin(115200);

  registers.asInt = 0b00000010;
  shiftOut32();
  // Set hostname.
  WiFi.hostname(HOSTNAME);
  // WiFi
  // Local initialization. Once its business is done, there is no need to keep
  // it around.
  WiFiManager wifiManager;
  // reset saved settings
  // Do we need to reset out wifi settings?
  if(getButtonState(1) && getButtonState(2)) {
    Serial.println("Resetting wifi settings.");
    wifiManager.resetSettings();
    ESP.restart();
  }
  // fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("binClock");
  // if you get here you have connected to the WiFi
  Serial.println("Connected to WiFi. Starting.");
  registers.asInt = 0b00000100;
  shiftOut32();
  // Print out the IP.
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  // Then set up MDNS
  setupMdns();
  // Set up button callbacks.
  button1.onPressed(onButton1Press);
  button2.onPressed(onButton2Press);
  // Set registers to 1 for starters.
  registers.asInt = 0b00001000;
  shiftOut32();
  // Initialise our modes.
  setupModeManager();
  // And we're done.
}

/**
 * Main application loop.
*/
void loop() {
  modeManager.loopActiveMode(registers, millis());
  shiftOut32();
  // Process buttons.
  button1.read();
  button2.read();
}
