#pragma once
#define ledOut 12
#include "Arduino.h"


extern uint8_t brightness;
extern bool lightIsOn;


void changeBrightness(uint8_t brightnessValue);
void changeLightState(uint8_t state);
