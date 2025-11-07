#include <ESP8266WiFi.h>
#include "wifi.h"
#include "config.h"

void connectToWifi() {
  WiFi.begin(ssid, password); // config.h
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
}

