#include <Arduino.h>

// WiFi
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <mDNS.h>

#include <EasyButton.h>

// Time
// https://randomnerdtutorials.com/esp32-ntp-timezones-daylight-saving/
#include "time.h"
const String TZ = "GMT0BST,M3.5.0/1,M10.5.0;";

// Pins
#define INTERNAL_LED 8
#define CLOCK 5
#define LATCH 6
#define DATA 7

// Buttons
#define BUTTON1 9
#define BUTTON2 8
EasyButton button1(BUTTON1);
EasyButton button2(BUTTON2);

// Flags
#define WRITEMSBFIRST
#define DEBUGno
#define USEARDUINOSHIFTno

// Config
#define HOSTNAME "binClock"

// Registers
struct RegistersStruct
{
  uint8_t register1;
  uint8_t register2;
  uint8_t register3;
  uint8_t register4;
};

union RegistersUnion
{
  uint32_t asInt;
  RegistersStruct asStruct;
};

RegistersUnion registers;

// Mode and mode globals.
enum Mode { binClock, pingpong, showIp };
Mode modes[3] = {
  binClock, pingpong, showIp
};
Mode mode = binClock;
bool pingPongLeft = true;

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
void shiftOut32()
{
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

void ArduinoShiftOut32()
{
  digitalWrite(LATCH, LOW);
  digitalWrite(CLOCK, LOW);
  shiftOut(DATA, CLOCK, LSBFIRST, registers.asStruct.register1);
  digitalWrite(CLOCK, LOW);
  shiftOut(DATA, CLOCK, LSBFIRST, registers.asStruct.register2);
  digitalWrite(CLOCK, LOW);
  shiftOut(DATA, CLOCK, LSBFIRST, registers.asStruct.register3);
  digitalWrite(CLOCK, LOW);
  shiftOut(DATA, CLOCK, LSBFIRST, registers.asStruct.register4);
  digitalWrite(LATCH, HIGH);
}

// Sets TZ from POSIX style TZ.
// See: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
void setTimezone(String timezone)
{
  Serial.printf("Setting Timezone to %s\n", timezone.c_str());
  setenv("TZ", timezone.c_str(), 1); //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
  tzset();
}

void initTime(String timezone)
{
  struct tm timeinfo;

  Serial.println("Setting up time");
  configTime(0, 0, "pool.ntp.org"); // First connect to NTP server, with 0 TZ offset
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("  Failed to obtain time");
    return;
  }
  Serial.println("  Got the time from NTP");
  // Now we can set the real timezone
  setTimezone(timezone);
}

String getModeAsString(Mode mode) {
  switch(mode) {
    case binClock:
      return "binClock";
    case pingpong:
      return "pingpong";
    case showIp:
      return "showIp";
    default:
      return "Unknown mode";
  }
}

// Cycle through modes.
void onButton1Press() {
  // Loop through modes.
  // Don't check the last item in the array. If we didn't find the current mode
  // in the array we'll assume it was the last, and set mode to the 0 position.
  Serial.println("Switching modes.");
  // reset the registers.
  registers.asInt = 1;
  // Calculate how many modes there are (compiler should be able to figure this
  // all out at compile time.)
  for(uint8_t i = 0; i < (sizeof(modes) / sizeof(Mode)) - 1; i++){
    Serial.println(i);
    if(modes[i] == mode) {
      Serial.printf("Matched mode %s, setting to %s.\n", getModeAsString(mode).c_str(), getModeAsString(modes[i+1]).c_str());
      mode = modes[i+1];
      
      return;
    }
  }
  Serial.printf("Didn't match mode, setting to %s.", getModeAsString(modes[0]).c_str());
  mode = modes[0];
}

void startMdns()
{
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

void setup()
{
  // Pins
  pinMode(INTERNAL_LED, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(LATCH, OUTPUT);
  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
  digitalWrite(CLOCK, LOW);
  // Set leds to init.
  registers.asInt = 1;
  shiftOut32();
  // Serial
  Serial.begin(115200);

  registers.asInt = 3;
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
  registers.asInt = 7;
  // Print out the IP.
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  // Then set up MDNS
  startMdns();
  // Getting time.
  initTime(TZ);
  // Set up button callbacks.
  button1.onPressed(onButton1Press);
  // Set registers to 1 for starters.
  registers.asInt = 1;
}

void modeClock() {
  // Get the latest time.
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time 1");
    // Turn on all LEDS to signal an error.
    registers.asInt = UINT32_MAX;
  }
  registers.asStruct.register1 = timeinfo.tm_hour;
  registers.asStruct.register2 = timeinfo.tm_min;
  registers.asStruct.register3 = timeinfo.tm_sec;

  #ifdef DEBUG
    Serial.printf("%02d:%02d:%02d\n", registers.asStruct.register1, registers.asStruct.register2, registers.asStruct.register3);
  #endif

  #ifdef USEARDUINOSHIFT
    ArduinoShiftOut32();
  #else
    shiftOut32();
  #endif

  delay(1000);
}

// Makes an LED go back and forth.
void modePingPong() {
  // Do we need to change dir?
  if(registers.asInt & 1) {
    pingPongLeft = true;
  // 2147483648 = 2^31 i.e. highest bit on register set.
  }
  // Then shift.
  if(pingPongLeft) {
    registers.asInt = registers.asInt << 1;
  } else {
    registers.asInt = registers.asInt >> 1;
  }
  #ifdef USEARDUINOSHIFT
    ArduinoShiftOut32();
  #else
    shiftOut32();
  #endif
  // Reverse other way?
  if(registers.asInt & 2147483648) {
    pingPongLeft = false;
  }
  delay(100);
}

// Shows the device IP on the LEDS.
void modeShowIp() {
  registers.asInt = WiFi.localIP();
  #ifdef USEARDUINOSHIFT
    ArduinoShiftOut32();
  #else
    shiftOut32();
  #endif
  delay(2000);
}

// Our main loop/
void loop() {
  // Run the appropriate mode.
  switch(mode) {
    case binClock:
      modeClock();
      break;
    case pingpong:
      modePingPong();
      break;
    case showIp:
      modeShowIp();
      break;
    default:
      modeClock();
      break;
  }
  // Process buttons.
  button1.read();
  button2.read();
}
