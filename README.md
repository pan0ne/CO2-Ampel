***CO2-Messgerät/Ampel***

![CO2-Ampel_Galerie](images/CO2-Meter2.jpg)

[Weitere Fotos in der Galerie auf Thingiverse](https://www.thingiverse.com/thing:4644826)

Dieses Projekt zeigt den Bau einer CO2 Ampel.
Benutzt wird ein ESP8266 / ESP32 Mikrocontroller und ein MHZ-19b CO2 Sensor. Zusätzlich kann ein BME680 Luftqualitätssensor angeschlossen werden um die Messwerte zu präzisieren sowie Neopixel und Display zur visuellen Ausgabe.  

Gemessen werden Temperatur (C), Feuchte (%), CO2 (ppm), Luftdruck(hPa) und die Luftqualität (flüchtige organische Verbindungen, VOC).

Die Sensordaten können via **LoRa WAN ** über thethingsnetwork- oder direkt per WiFi an  thingsspeak.com übertragen auf  grafisch aufbereitet werden.

Bis auf den CO2 Sensor, der mit 5V betrieben wird, reicht für alles andere 3.3V. Der CO2 Sensor wird seriell über die RX/TX Pins abgefragt. Der Luftqualitätssensor kommuniziert über den i2c bus.

**3D Druckmodelle**
(Design mit TinkerCad.com):

Download hier: https://www.thingiverse.com/thing:4644826

- *Material*:   PLA (Sunlu), Holzoptik, carbonschwarz, transparent
- *Temperatur*: 195°C Noozle / Heizbett 58°
- *Einstellungen*: keine Stützstrukturen
- *Slicer*: Cura 4.8.0

Eine Anleitung mit allen Angaben zur Verkabelung und Programmierung im Github Wiki:

- https://github.com/pan0ne/CO2-Ampel/wiki/Anleitung
