***CO2-Messgerät/Ampel***

![CO2-Ampel_Galerie](images/CO2-Meter2.jpg)

[Weitere Fotos in der Galerie auf Thingiverse](https://www.thingiverse.com/thing:4644826)

Dieses Projekt zeigt den Bau einer CO2 Ampel.
Benutzt wird ein ESP8266 / ESP32 Mikrocontroller und ein MHZ-19b CO2 Sensor. Zusätzlich kann ein BME680 Luftqualitätssensor angeschlossen werden um die Messwerte zu präzisieren sowie Neopixel und Display zur visuellen Ausgabe.  

Gemessen werden Temperatur (C), Feuchte (%), CO2 (ppm), Luftdruck(hPa) und die Luftqualität (flüchtige organische Verbindungen, VOC).

Die Sensordaten können via **LoRa WAN ** über thethingsnetwork- oder direkt per WiFi an  thingsspeak.com übertragen auf  grafisch aufbereitet werden.

Bis auf den CO2 Sensor, der mit 5V betrieben wird, reicht für alles andere 3.3V. Der CO2 Sensor wird seriell über die RX/TX Pins abgefragt. Der Luftqualitätssensor kommuniziert über den i2c bus. 

**Gehäuse** (Alle Bauteile wurden mit TinkerCad.com entworfen)

https://www.thingiverse.com/thing:4644826
(Alle Druckvorlagen sind Prototypen und befinden sich im Repository, nicht alle sind vollständig getestet.)

Je nach Druckeinstellungen und Material können die Ergebnisse etwas abweichen. Als Material wurde PLA Filament in Holzoptik (Sunlu, Farbe Buche), Carbonschwarz sowie transparentes Material (mit photolumineszierenden/phosphoreszierenden Pigmenten) benutzt. Temperatur zwischen 195C und 200C im HotEnd sowie 58C - 60C auf dem Druckbett. Gedruckt mit einem AnyCubic I3 Mega und Cura 4.7.1 als Slicer. Ohne Stützstrukturen gedruckt.
Achtung: Es befinden sich weitere Gehäuseteile für die WiFi Variante im Verzeichnis, allerdings sind diese Druckvorlagen noch nicht vollständig getestet.
*Druckdauer für alle Druckobjekte: ca. ~8 Stunden*

**Verwendete Libraries/ Sketch (Arduino IDE):**                                        
-  Alle Libraries, die im Sketch benutzt werden, können über den Library Manager der Arduino IDE installiert werden.

**Boardmanager for Heltec ESP32 Mikrocontroller:**
- Alternative: https://resource.heltec.cn/download/package_heltec_esp32_index.json (newer and more examples)
- https://dl.espressif.com/dl/package_esp32_index.json

**Boardmanager for ESP8266 Mikrocontroller:**
http://arduino.esp8266.com/stable/package_esp8266com_index.json

**Wiring/Verkabelung:**

| BME680 | Heltec | ESP8266|
| ------ | ------ |-------|
| 3,3V | 3,3V | 3,3V |
| GND | GND | GND |
| SDA | 22 | D2 |
| SCL | 21 | D1 |

| MH-Z19B | Heltec | ESP8266
| ------ | ------ |-------|
| 5V | 5V | Vin
| GND | GND | GND|
| TX | RX PIN | RX Pin
| RX | TX PIN | TX Pin

| Neopixel WS2812 | Heltec | ESP8266
| ------ | ------ |------ |
| GND | GND | GND |
| Din | 25 | D8 |
| 3,3V | 3,3V | 3,3V|

| Touch-Button | Heltec | ESP8266
| ------ | ------ | ------|
| GND | GND | GND |
| Din | 13 | D7 |
| 3,3V | 3,3V | 3,3V

**Kabelfarben:**

- 5V / GND = rot/schwarz
- 3.3V / GND = weiß/schwarz
- i2c sda = blau
- i2c sck = grün
- Seriell RX = orange
- Seriell TX = gelb
- Neopixel = lila
- Touch-Button = grau

**Fritzing für ESP32 (Heltec Wifi LoRa v2)**

![CO2-Ampel_Schema](images/CO2Ampel_WiringDiagram.png)

**Fritzing für ESP8266 (Amica)**

![CO2-Ampel_ESP8266_Schema](Docs/Schema_ESP8266.jpeg)
