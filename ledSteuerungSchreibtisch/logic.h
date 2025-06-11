#pragma once
#include <map>
#include "Arduino.h"
#include "lightController.h"

extern bool lastAccessSourceWeb;

int getPotiValues();
std::map<String, String> parseParams(const String& uri);