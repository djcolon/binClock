#include <Arduino.h>

// WiFi
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager

// Time
// https://randomnerdtutorials.com/esp32-ntp-timezones-daylight-saving/
#include "time.h"
const String TZ = "GMT0BST,M3.5.0/1,M10.5.0;";

// Pins
#define INTERNAL_LED D4
#define CLOCK D0
#define LATCH D1
#define DATA D2

// Flags
#define WRITEMSBFIRST
#define DEBUG
#define USEARDUINOSHIFT

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
      bit = !!(registers.asInt & (1 << (32 - i)));
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
  shiftOut(DATA, CLOCK, MSBFIRST, registers.asStruct.register1);
  digitalWrite(CLOCK, LOW);
  shiftOut(DATA, CLOCK, MSBFIRST, registers.asStruct.register2);
  digitalWrite(CLOCK, LOW);
  shiftOut(DATA, CLOCK, MSBFIRST, registers.asStruct.register3);
  digitalWrite(CLOCK, LOW);
  shiftOut(DATA, CLOCK, MSBFIRST, registers.asStruct.register4);
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

void setup()
{
  // Pins
  pinMode(INTERNAL_LED, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(LATCH, OUTPUT);
  digitalWrite(CLOCK, LOW);
  // Set leds to init.
  registers.asStruct.register2 = 0;
  shiftOut32();
  // Serial
  Serial.begin(115200);

  registers.asStruct.register2 = 1;
  shiftOut32();
  // WiFi
  // Local initialization. Once its business is done, there is no need to keep
  // it around.
  WiFiManager wifiManager;
  // reset saved settings
  // wifiManager.resetSettings();

  // fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("binClock");
  // if you get here you have connected to the WiFi
  Serial.println("Connected to WiFi. Starting.");
  registers.asStruct.register2 = 4;
  // Getting time.
  initTime(TZ);
}

void loop()
{
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
