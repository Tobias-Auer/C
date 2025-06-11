#include "logic.h"
#include "Arduino.h"


int oldRaw = 0;
int oldValue = 0;
bool lastAccessSourceWeb = false;



int getPotiValues() {
  const int samples = 10;
  long sum = 0;

  int raw = analogRead(A0);
  if (raw < 570) return 0;
  Serial.println(raw);
  if (abs(oldRaw - raw) < 10) {
    if (lastAccessSourceWeb) {return brightness;}
    return oldValue;
  }
  lastAccessSourceWeb = false;
  oldRaw = raw;

  if (raw < 550) {
    raw = 550;
  } else if (raw > 970) {
    raw = 970;
  }
  oldValue = map(raw, 550, 970, 0, 255);
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