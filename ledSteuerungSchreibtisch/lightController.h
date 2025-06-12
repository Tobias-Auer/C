#pragma once
#define ledOut 14
#include "Arduino.h"


extern uint8_t brightness;
extern bool lightIsOn;

extern int globalPotiValue;

void changeBrightness(uint8_t brightnessValue);
void changeLightState(uint8_t state);
