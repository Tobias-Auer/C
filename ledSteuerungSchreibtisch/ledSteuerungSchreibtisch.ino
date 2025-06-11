#include "webserver.h"
#include "wifi.h"
#include "logic.h"
#include "lightController.h"

void setup() {
  Serial.begin(115200);
  connectToWifi();
  setupWebServer();
}

unsigned long lastBrightnessUpdateTime = 0;
const unsigned long brightnessUpdateInterval = 200; 

unsigned long loopIterations = 0;
unsigned long lastOverloadCheckTime = 0;

void loop() {
  loopIterations++;

  unsigned long currentTime = millis();
  if (currentTime - lastOverloadCheckTime >= 1000) {
    if (loopIterations < 50000) {
      Serial.println("WARNING! OVERLOAD DETECTED!");
    }
    loopIterations = 0;
    lastOverloadCheckTime = currentTime;
  }
  
  server.handleClient();

  currentTime = millis();
  if (currentTime - lastBrightnessUpdateTime >= brightnessUpdateInterval) {
    lastBrightnessUpdateTime = currentTime;
    changeBrightness(getPotiValues());
  }
}
