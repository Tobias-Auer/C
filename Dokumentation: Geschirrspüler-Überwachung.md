# Dokumentation: Geschirrspüler-Überwachungssoftware für ESP8266

Diese Dokumentation beschreibt die Funktionsweise und Konfiguration eines Programms für einen **ESP8266 ESP-01M**, das zur Überwachung eines Geschirrspülers mithilfe eines Lichtsensors entwickelt wurde. Der Status des Geschirrspülers wird analysiert und an ein Loxone-System über verschiedene HTTP-Requests gemeldet. Das Programm bietet einen **Setup-Modus** für die Konfiguration und einen **Run-Modus** für den Normalbetrieb.

---

## Aufbau und Funktionsweise

### Setup-Modus
Im Setup-Modus agiert der ESP als Access Point und hostet eine Konfigurationswebseite, auf der alle erforderlichen Einstellungen vorgenommen werden können. Dieser Modus wird aktiviert, wenn der **`enterSetupPin`** beim Start LOW ist. Das wird erreicht, indem der Pushbutton beim Starten des Programms gedrückt wird. 
> SSID: ESP-AP
>
> Passwort: 12345678

Auf der Konfigurationsseite können folgende Felder ausgefüllt werden:

- **SSID**: Der Name des WLANs, mit dem sich der ESP verbinden soll.
- **Wifi Password**: Das Passwort für die Verbindung zum WLAN.
- **Request On**: Der HTTP-Pfad, der gesendet wird, wenn ein Zustand "ON" erkannt wird (z. B. Spülvorgang aktiv).
- **Request Off**: Der HTTP-Pfad, der gesendet wird, wenn ein Zustand "OFF" erkannt wird (z. B. Spülvorgang beendet).
- **Request Startup**: Der HTTP-Pfad, der gesendet wird, wenn der ESP erfolgreich startet.
- **Request Error**: Der HTTP-Pfad, der bei einem Fehler (z. B. Watchdog Reset) gesendet wird.
- **Request Heartbeat**: Ein HTTP-Pfad, der in regelmäßigen Abständen (24h) gesendet wird, um anzuzeigen, dass der ESP aktiv ist.
- **Username**: Der Benutzername für die Authentifizierung mit Loxone.
- **Loxone Password**: Das Passwort für die Authentifizierung mit Loxone.
- **IP**: Die IP-Adresse des Loxone-Servers.
- **Port**: Der Port, über den Loxone erreichbar ist.
- **Threshold**: Der Schwellenwert für die Differenz des Lichtsensorsignals, der entscheidet, ob ein Statuswechsel erkannt wird.

### Run-Modus
Im Run-Modus vergleicht der ESP alle 60 Sekunden den aktuellen Sensorwert mit dem letzten. Sollte eine Änderung über den konfigurierten Threshold geben, verbindet sich der ESP mit dem WLAN-Netzwerk.
Sollte es keine entscheidende Änderung geben, verbindet sich der ESP nicht mit dem WLAN-Netzwerk und geht wieder für 60 Sekunden in den Deep-Sleep Modus.
Wenn der ESP sich mit dem WLAN verbunden hat, da es eine entscheidende Änderung im Sensorwert gab, wird ein HTTP-Request gesendet. Diese werden in den Feldern `Request On` und `Request Off` konfiguriert.

---

## Detailierte Feldbeschreibungen

### WLAN-Konfiguration
- **SSID**: Der Name des WLANs, das für die Netzwerkverbindung genutzt wird.
- **WiFi Password**: Das zugehörige Passwort für das Netzwerk.

### HTTP-Requests
Diese Felder legen die Aktionen fest, die an den Loxone-Server gesendet werden:
- **Request On**: Wird gesendet, wenn der konfigurierte Threshold unterschritten wird.
- **Request Off**: Wird gesendet, wenn der konfigurierte Threshold überschritten wird.
- **Request Startup**: Zeigt an, dass das ESP nach einem Neustart aufgrund eines Fehlers oder eines Stromausfalls wieder ordnungsgemäß funktioniert.
- **Request Error**: Wird im Falle eines Fehlers (Hardware Watchdog Reset, Exception, Programm Watchdog Reset) gesendet.
- **Request Heartbeat**: Zeigt an, dass der ESP aktiv ist. Die Anfrage wird alle 24 Stunden unabhängig vom Status des Lichtzustands gesendet.

### Loxone-Server-Konfiguration
- **Username**: Der Benutzername für die HTTP-Authentifizierung.
- **Loxone Password**: Das zugehörige Passwort.
- **IP**: Die IP-Adresse des Loxone-Servers (z. B. `192.168.1.100`).
- **Port**: Der Port des Servers (Standard für HTTP ist `80`, für HTTPS `443`).

>**Wichtiger Hinweis:** Es empfiehlt sich hierfür einen dedizierten Nutzer in Loxone anzulegen, da die Verbindung unverschlüsselt ist und andere Geräte im Netzwerk die Authentifizierungsdaten abfangen können!

### Schwellenwert für Lichtsensor
- **Threshold**: Gibt die Empfindlichkeit für Änderungen am Lichtsensorwert an. Höhere Werte bedeuten, dass größere Unterschiede notwendig sind, um einen Statuswechsel auszulösen.

---

## Technische Implementierungen

### EEPROM-Nutzung
Das Programm verwendet den **EEPROM**, um Einstellungen persistent zu speichern. Es ist möglich, diese Werte im Setup-Modus zu aktualisieren. Beim Start des ESP werden die gespeicherten Einstellungen aus dem EEPROM geladen. 

- **EEPROM-Größe**: 512 Bytes.
- Die Konfiguration darf aufgrund von einem kleinen Eepromspeicher nicht größer als 500 Byte sein. Sollten die Konfigurationsdaten größer sein, wird es zu einem Fehler führen und die aktuelle Konfiguration wird nicht übernommen werden!

### Lichtsensor
Der Lichtsensor ist mit **Pin `A0`** verbunden. Der gemessene Wert wird in regelmäßigen Abständen mit dem letzten gespeicherten Wert verglichen, um Änderungen zu erkennen:
- Ein Anstieg, der einen höheren Wert als den definierten Schwellenwert aufweist, führt zur Aktivierung des **`OFF`**-Status.
- Ein Anstieg, dessen Wert geringer ist als der festgelegte Schwellenwert, führt zur Aktivierung des **`ON`**-Status.


### Deep-Sleep-Modus
Um Energie zu sparen, nutzt der ESP den Deep-Sleep-Modus:
- Der ESP wacht alle **60 Sekunden** auf, misst den Sensorwert und verarbeitet den Zustand.
- Zwischen den Messungen wird der Wert des Lichtsensors im EEPROM gespeichert.

### Reset-Gründe
Das Programm kann den Grund für den Neustart des ESP erkennen und darauf reagieren:
- Bei kritischen Fehlern (z. B. Hardware Watchdog) wird der **Error Request** gesendet.
- Bei regulären Neustarts, ausgelöst durch Powerloss zum Beispiel, wird der **Startup Request** gesendet.

### Webserver
Im Setup-Modus wird ein Webserver gestartet, der folgende Funktionen bietet:
- Einstellungen unter `/`
- Abrufen der aktuellen ADC-Werte unter `/adc`.
- Absenden der Konfiguration über `/submit`.

### HTTP-Requests an Loxone
Das Programm sendet HTTP-GET-Anfragen an die API des Loxone-Servers. Die Authentifizierung erfolgt durch den Benutzername und das Passwort in der URL.

Implementierung:
```plaintext
http://<username>:<loxone_password>@<ip>:<port>/dev/sps/io/<actionName>
```