#include "webserver.h"
#include "logic.h"
#include "lightController.h"


ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

void handleJsonReturn() {
String json = String("{\"bri\": ") + brightness + ", \"on\":" + lightIsOn + ", \"val\":" + globalPotiValue + "}";
  server.send(200, "application/json", json);
}

#include <map>

std::map<String, String> params;

void handleWin(const std::map<String, String>& params) {
  if (params.count("A") > 0) {
    changeBrightness(params.at("A").toInt());
    lastAccessSourceWeb = true;
    }
  if (params.count("T") > 0) {
    Serial.print("SWITCHING MODE: ");
    Serial.println(params.at("T").toInt());
    changeLightState(params.at("T").toInt());
  }

  server.send(200, "text/plain", "");
}

void handleRoot() {
  server.send(200, "text/plain", "Hallo von ESP8266");
}


void setupWebServer() {
  httpUpdater.setup(&server);

  server.on("/", handleRoot);
  // server.on("/win", handleWin);                // /win&T=0/1/2         /win&A=128
  server.on("/json/state", handleJsonReturn);  //{"on": true, "bri": 128}
  server.begin();
  Serial.println("HTTP Server initialisiert und gestartet");
  server.onNotFound([]() {
  String uri = server.uri();
  if (uri.startsWith("/win&")) {
    handleWin(parseParams(uri));

  } else {
    server.send(404, "text/plain", "Not found");
  }
});

}