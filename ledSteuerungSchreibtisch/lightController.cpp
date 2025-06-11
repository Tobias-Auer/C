#include "lightController.h"

uint8_t oldBrightness = 0;
uint8_t brightness = 0;
bool lightIsOn = true;

void changeLightState(uint8_t state) {
  Serial.print(("RECEIVED: "));
  Serial.println(state);
  switch (state) {
    case 0:
      lightIsOn = false;
      break;
    case 1:
      lightIsOn = true;
      break;
    case 2:
      lightIsOn = !lightIsOn;
      break;
  }
}


void changeBrightness(uint8_t brightnessLevel) {
  brightness = brightnessLevel;
  if (abs(brightness - oldBrightness) < 2) {
    return;
  }
  oldBrightness = brightness;
  Serial.println(lightIsOn);
  if (lightIsOn) {
    Serial.print("BRIGHTNESS: ");
    Serial.println(brightness);
      analogWrite(ledOut, brightness);
  } else {
    analogWrite(ledOut, 0);
    Serial.println("OFFFFFFFFFFFFFFFFFFFFFF");
  }
}

