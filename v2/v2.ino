#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <base64.h>
#include <ESP8266HTTPClient.h>

const int enterSetupPin = 4;
const int adcPin = A0;

const int groundPinVD = 14;
const int powerPinVD = 12;

int thresholdDelta;
int previousValue;
int wakeUpsUntilBeat;

String savedSSID = "";
String savedWIFIPassword = "";
String savedRequestOn = "";
String savedRequestOff = "";
String savedRequestStartup = "";
String savedRequestError = "";
String savedHeartbeat = "";
String savedUsername = "";
String savedLOXONEPassword = "";
String savedIp = "";
int savedPort = 0;

bool doSetup = false;

AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>ESP Webserver</title>
<script>
async function fetchADCValue() {
const response = await fetch('/adc');
const adcValue = await response.text();
document.getElementById('status').innerText = adcValue;
}async function submitForm() {
const ssid = document.getElementById('ssid').value;
const passwordW = document.getElementById('passwordW').value;
onRequest = document.getElementById('requestOn').value;
const offRequest = document.getElementById('requestOff').value;
const threshold = document.getElementById('threshold').value;
const requestStartup = document.getElementById('requestStartup').value;
const passwordL = document.getElementById('passwordL').value;
const username = document.getElementById('username').value;
const ip = document.getElementById('ip').value;
const port = document.getElementById('port').value;
const heartbeat = document.getElementById('heartbeat').value;
const requestError = document.getElementById('requestError').value;
await fetch('/submit', {
method: 'POST',
headers: { 'Content-Type': 'application/json' },
body: JSON.stringify({ ssid, passwordW, onRequest, offRequest, threshold, requestStartup, username, passwordL, ip, port, heartbeat, requestError })
});
alert('Settings saved');
}
setInterval(fetchADCValue, 500); // Fetch ADC value every half second
</script>
</head>
<body>
<h1>ESP Webserver</h1>
<div>
<label for="ssid">SSID:</label>
<input type="text" id="ssid" value="{{ssid}}">
</div>
<div>
<label for="passwordW">Wifi Password:</label>
<input type="text" id="passwordW" value="{{passwordW}}">
</div>
<div>
<label for="requestOn">Request On:</label>
<input type="text" id="requestOn" value="{{onRequest}}">
</div>
<div>
<label for="requestOff">Request Off:</label>
<input type="text" id="requestOff" value="{{offRequest}}">
</div>
<div>
<label for="requestStartup">Request Startup:</label>
<input type="text" id="requestStartup" value="{{requestStartup}}">
</div>
<div>
<label for="heartbeat">Request Heartbeat:</label>
<input type="text" id="heartbeat" value="{{heartbeat}}">
</div>
<div>
<label for="requestError">Request Error:</label>
<input type="text" id="requestError" value="{{requestError}}">
</div>
<div>
<label for="username">Username:</label>
<input type="text" id="username" value="{{username}}">
</div>
<div>
<label for="passwordL">LoxOne Password:</label>
<input type="text" id="passwordL" value="{{passwordL}}">
</div>
<div>
<label for="ip">Ip:</label>
<input type="text" id="ip" value="{{ip}}">
</div>
<div>
<label for="port">Port:</label>
<input type="text" id="port" value="{{port}}">
</div>
<div>
<label for="threshold">Threshold:</label>
<input type="text" id="threshold" value="{{threshold}}">
</div><div><button onclick="submitForm()">Submit</button>
</div><div><p>Status: <span id="status">Unknown</span></p></div>
</body>
</html>
)rawliteral";

void setAllPinsToLow() {
  pinMode(groundPinVD, INPUT);
  pinMode(powerPinVD, INPUT);
}


void loadSettingsFromEEPROM() {
  char buffer[500];  // Adjust size to match EEPROM size
  EEPROM.get(0, buffer);
  if (buffer[0] != '\0') {  // Ensure EEPROM is not empty
    Serial.println("Loaded flash data:");
    StaticJsonDocument<512> jsonDoc;
    deserializeJson(jsonDoc, buffer);

    savedSSID = jsonDoc["ssid"] | "";
    savedWIFIPassword = jsonDoc["passwordW"] | "";
    savedRequestOn = jsonDoc["onRequest"] | "";
    savedRequestOff = jsonDoc["offRequest"] | "";
    savedRequestStartup = jsonDoc["requestStartup"] | "";
    savedRequestError = jsonDoc["requestError"] | "";
    thresholdDelta = atoi(jsonDoc["threshold"] | "100");
    savedUsername = jsonDoc["username"] | "";
    savedLOXONEPassword = jsonDoc["passwordL"] | "";
    savedIp = jsonDoc["ip"] | "";
    savedPort = atoi(jsonDoc["port"] | "0");
    savedHeartbeat = jsonDoc["heartbeat"] | "";
  }
  int dataSize = strlen(buffer);
  Serial.print("Länge der geladenen JSON-Daten: ");
  Serial.println(dataSize);
  Serial.println("----");
  Serial.println(savedSSID);
  Serial.println(savedWIFIPassword);
  Serial.println(savedRequestOn);
  Serial.println(savedRequestOff);
  Serial.println(savedRequestStartup);
  Serial.println(savedHeartbeat);
  Serial.println(thresholdDelta);
  Serial.println(savedUsername);
  Serial.println(savedLOXONEPassword);
  Serial.println(savedIp);
  Serial.println(savedPort);
  Serial.println("----");

  int loadedValue;
  EEPROM.get(504, loadedValue);
  if (buffer[0] != '\0') {  // Ensure EEPROM is not empty
    Serial.println("Loaded more flash data:");
    Serial.println(loadedValue);
    previousValue = loadedValue;
  }

  EEPROM.get(508, wakeUpsUntilBeat);
  if (buffer[0] != '\0') {  // Ensure EEPROM is not empty
    Serial.println(wakeUpsUntilBeat);
    Serial.println("----");
  } else {
    wakeUpsUntilBeat = 0;
  }
}

String checkLightSensor() {
  int currentLightValue = analogRead(adcPin);
  int delta = currentLightValue - previousValue;
  if (!doSetup)  {
    EEPROM.put(504, currentLightValue);
  } else {
    previousValue = currentLightValue;
  }
  if (wakeUpsUntilBeat <= 0) {
    if (!doSetup) {EEPROM.put(508, 1440);  EEPROM.commit();}
    sendHTTPRequest(savedHeartbeat);
  } else {
    if (!doSetup) { EEPROM.put(508, wakeUpsUntilBeat - 1);   EEPROM.commit();}
    
  }

  if (delta > thresholdDelta) {
    sendHTTPRequest(savedRequestOff);
    return "off";
  } else if (delta < -thresholdDelta) {
    sendHTTPRequest(savedRequestOn);
    return "on";
  }
  Serial.println("nothing");
  return "nothing";
}


void setup() {
  Serial.begin(115200);
  Serial.println("Started programm");
  Serial.println(ESP.getFlashChipId(), HEX);
  String resetReason = ESP.getResetReason();
  Serial.println("Reset Reason: " + resetReason);
  EEPROM.begin(512);
  pinMode(enterSetupPin, INPUT_PULLUP);
  pinMode(adcPin, INPUT);
  pinMode(groundPinVD, OUTPUT);
  pinMode(powerPinVD, OUTPUT);
  digitalWrite(groundPinVD, LOW);
  digitalWrite(powerPinVD, HIGH);

  if (digitalRead(enterSetupPin) == LOW) {
    doSetup = true;
  }
  loadSettingsFromEEPROM();

  if (doSetup) {
    // Im Setup-Modus einen Access Point starten
    WiFi.softAP("ESP-AP", "12345678");  // Setze den Access Point Namen und das Passwort
    Serial.println("Access Point started");
    Serial.println("AP IP address: " + WiFi.softAPIP().toString());

    // Webserver-Routen im Setup-Modus definieren
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      String html = String(index_html);  // Make a copy of the HTML template
      html.replace("{{ssid}}", savedSSID);
      html.replace("{{passwordW}}", savedWIFIPassword);
      html.replace("{{onRequest}}", savedRequestOn);
      html.replace("{{offRequest}}", savedRequestOff);
      html.replace("{{requestStartup}}", savedRequestStartup);
      html.replace("{{heartbeat}}", savedHeartbeat);
      html.replace("{{requestError}}", savedRequestError);
      html.replace("{{passwordL}}", savedLOXONEPassword);
      html.replace("{{username}}", savedUsername);
      html.replace("{{threshold}}", String(thresholdDelta));
      html.replace("{{ip}}", savedIp);
      html.replace("{{port}}", String(savedPort));


      request->send(200, "text/html", html);
    });

    server.on(
      "/submit", HTTP_POST,
      [](AsyncWebServerRequest *request) {}, NULL,
      [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        // Allocate memory for JSON data on the first chunk
        if (index == 0) {
          if (request->_tempObject) delete request->_tempObject;  // Free previous request data, if any
          request->_tempObject = new String();
        }

        // Cast and append the current chunk to the accumulated JSON data
        String *jsonData = static_cast<String *>(request->_tempObject);
        *jsonData += String((char *)data).substring(0, len);

        // Check if all chunks are received
        if (index + len == total) {
          Serial.println("Received full JSON: " + *jsonData);

          // Process JSON data
          if (jsonData->length() > 500) {
            Serial.println("JSON data too large to fit in EEPROM!");
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Data too large\"}");
          } else {
            // Write the JSON to EEPROM
            for (int i = 0; i < jsonData->length(); i++) {
              EEPROM.write(i, (*jsonData)[i]);
            }
            EEPROM.commit();

            request->send(200, "application/json", "{\"status\":\"saved\"}");
          }

          // Clean up after processing
          delete request->_tempObject;
          request->_tempObject = nullptr;
          loadSettingsFromEEPROM();
        }
      });


    server.on("/adc", HTTP_GET, [](AsyncWebServerRequest *request) {
      int adcValue = analogRead(adcPin);
      request->send(200, "text/plain", String(adcValue) + ", " + checkLightSensor());
    });



    server.begin();  // Webserver im Setup-Modus starten
  } else {
    if (resetReason != "Deep-Sleep Wake") {
      EEPROM.put(508, 1440);
      EEPROM.commit();
      Serial.println("esp got restarted but not by deep sleep!\nChecking for errors...");
      if (resetReason == "Exception" || resetReason == "Watchdog" || resetReason == "Hardware WDT" || resetReason == "Restart on exception") {
        Serial.println("\n\n*****WARNING*****\n\nCritical error detected!\nTrying to report...\n");
        sendHTTPRequest(savedRequestError);
      } else {
        Serial.println("No errors found...\nSending startup request");
        sendHTTPRequest(savedRequestStartup);
      }
    }
  }
}

void loop() {
  if (!doSetup) {
    checkLightSensor();
    Serial.println("Sleep...");
    Serial.flush();
    setAllPinsToLow();
    delay(100);
    Serial.end();
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    delay(300);
    ESP.deepSleep(56e6);
  }
}

bool sendHTTPRequest(String actionName) {
  if (doSetup) { return true; }
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(savedSSID, savedWIFIPassword);  // WLAN mit SSID und Passwort verbinden
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
  }

  HTTPClient http;

  String url = "http://" + savedUsername + ":" + savedLOXONEPassword + "@" + savedIp + "/dev/sps/io/" + actionName;
  WiFiClient client;
  Serial.println("Connecting to: " + url);
  http.begin(client, url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println(payload);
    http.end();
    return true;
  } else {
    Serial.println("HTTP request failed");
    http.end();
    return false;
  }
}