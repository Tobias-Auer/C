#include "logic.h"
#include "Arduino.h"


int oldRaw = 0;
int oldValue = 0;
bool lastAccessSourceWeb = false;



int getPotiValues() {
  static unsigned long aboveThresholdStart = 0;
  static bool unlocked = false;
  const int threshold = 550;
  const unsigned long unlockDelay = 500;  // milliseconds
  int raw = analogRead(A0);
  Serial.println(raw);

  unsigned long now = millis();

  // --- Lock/unlock logic ---
  if (raw < threshold) {
    // Immediately lock and return 0
    unlocked = false;
    aboveThresholdStart = 0;
    oldValue = 0;
    globalPotiValue = raw;
    return 0;
  } else {
    // Above threshold
    if (!unlocked) {
      if (aboveThresholdStart == 0) {
        // Start counting
        aboveThresholdStart = now;
      } else if (now - aboveThresholdStart >= unlockDelay) {
        // Unlock after 500ms
        unlocked = true;
      }
    }
  }

  // --- If still locked, return 0 ---
  if (!unlocked) {
    oldValue = 0;
    return 0;
  }

  // --- Normal behavior once unlocked ---
  if (abs(oldRaw - raw) < 10) {
    if (lastAccessSourceWeb) {
      return brightness;
    }
    return oldValue;
  }

  lastAccessSourceWeb = false;
  oldRaw = raw;

  if (raw < 550) raw = 550;
  else if (raw > 985) raw = 985;

  oldValue = map(raw, 550, 985, 0, 255);
  globalPotiValue = raw;
  return oldValue;
}



std::map<String, String> parseParams(const String& uri) {
  std::map<String, String> params;

  int idx = 4;  // start after "/win"
  while (idx < uri.length()) {
    if (uri[idx] == '&') idx++;  // skip '&'
    if (idx >= uri.length()) break;

    // get key char (single char param name)
    String key = String(uri[idx]);
    idx++;

    // find next '&' or end of string for value substring
    int nextAmp = uri.indexOf('&', idx);
    if (nextAmp == -1) nextAmp = uri.length();

    String value = uri.substring(idx + 1, nextAmp);
    idx = nextAmp;

    params[key] = value;
  }
  return params;
}