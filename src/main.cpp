#include <Arduino.h>

#define INTERNAL_LED D4
#define MIN_CLOCK D0
#define MIN_LATCH D1
#define MIN_DATA D2
#define HR_CLOCK D5
#define HR_LATCH D6
#define HR_DATA D7

byte minRegister = 0;
// Our leds are on bit 1-7
byte minRegisterPos = 1;
byte hrRegister = 0;
byte hrRegisterPos = 1;

void updateMinRegister() {
  digitalWrite(MIN_LATCH, LOW);
  shiftOut(MIN_DATA, MIN_CLOCK, MSBFIRST, minRegister);
  digitalWrite(MIN_LATCH, HIGH);
}

void updateHrRegister() {
  digitalWrite(HR_LATCH, LOW);
  shiftOut(HR_DATA, HR_CLOCK, LSBFIRST, hrRegister);
  digitalWrite(HR_LATCH, HIGH);
}

void setup() {
  pinMode(INTERNAL_LED, OUTPUT);
  pinMode(MIN_CLOCK, OUTPUT);
  pinMode(MIN_LATCH, OUTPUT);
  pinMode(MIN_DATA, OUTPUT);
  pinMode(HR_CLOCK, OUTPUT);
  pinMode(HR_LATCH, OUTPUT);
  pinMode(HR_DATA, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  // Minutes
  if(minRegisterPos > 6) {
    minRegisterPos = 1;
    minRegister = 0;
  } else {
    bitSet(minRegister, minRegisterPos++);
    digitalWrite(INTERNAL_LED, minRegisterPos % 2 == 0 ? HIGH : LOW);
  }
  updateMinRegister();

  // Hours
  if(hrRegisterPos > 6) {
    hrRegisterPos = 1;
    hrRegister = 0;
  } else {
    bitSet(hrRegister, hrRegisterPos++);
  }
  updateHrRegister();

  delay(200);
}
