#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "secrets.h"
#include <ESP8266HTTPUpdateServer.h>


IRsend irsend(4);

uint16_t rawData[] = {3532,1726,486,382,486,1266,484,386,478,390,478,390,478,392,478,392,478,390,478,392,
                      478,390,478,390,478,392,478,392,478,1274,478,390,478,390,478,390,476,392,478,392,
                      478,390,478,390,478,1274,478,390,476,1276,478,390,478,390,478,1274,478,1274,478,
                      1274,478,390,478,390,478,390,478,1274,478,390,478,1274,478,1274,478,1274,478,1274,
                      478,390,478,392,478,1274,478,390,478,390,478,392,478,390,478,392,478,392,478,1274,476};

uint16_t rawData2[] = {3524,1732,478,390,480,1274,478,390,478,390,478,390,478,390,478,390,478,390,478,390,478,390,478,390,478,390,478,390,478,1274,478,390,478,390,478,390,478,390,478,390,478,390,480,390,478,1274,478,390,480,1274,478,390,478,390,478,1274,478,1274,480,1272,478,390,478,390,478,390,478,390,478,1274,478,1274,478,390,478,1274,478,390,478,390,478,1274,478,390,478,1274,478,390,478,1274,478,390,478,1274,478,390,478,392,476};

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;


void handleTrigger() {
  irsend.sendRaw(rawData, sizeof(rawData)/sizeof(rawData[0]), 38);
  delay(9000);
  irsend.sendRaw(rawData2, sizeof(rawData2)/sizeof(rawData2[0]), 38);
  server.send(200, "text/plain", "IR signal sent");
}

void handleHome() {
  server.send(200, "text/plain", "Welcome to radioControl. Visit /trigger to activate");
}

void setup() {
  Serial.begin(115200);
  irsend.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());

  server.on("/trigger", handleTrigger);
  server.on("/", handleHome);
      httpUpdater.setup(&server);

  server.begin();
}

void loop() {
  server.handleClient();
}
