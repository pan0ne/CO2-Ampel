# DIY CO2-Messgerät / CO2-Ampel

Dieses Projekt zeigt den Bau einer CO2-Ampel.
Als Basis benutze ich einen **ESP8266 / ESP32 Mikrocontroller** und ein **MHZ-19b CO2-Sensor**. Zusätzlich kann ein **BME680 Luftqualitätssensor** angeschlossen werden, um die Messwerte zu präzisieren sowie **Neopixel (WS2812b)** und/oder ein **SSD1306 Display** zur visuellen Anzeige.  

**Messdaten**
- **Temperatur (C)**
- **Luftfeuchtigkeit (%)**
- **CO2 (ppm)**,
- **Luftdruck (hPa)**
- **Luftqualität (VOC, flüchtige organische Verbindungen)**.

Die **Sensordaten** können via **LoRa WAN** an das **thethingsnetwork.org** gesendet werden oder direkt per **WiFi**.
Zur **Analyse der Daten** werden diese an **thingsspeak.com** übertragen und grafisch aufbereitet. (Account nötig)
Bis auf den CO2-Sensor, der mit 5V betrieben wird, reicht für alles andere 3.3V. Der CO2-Sensor wird seriell über die RX/TX Pins abgefragt. Der Luftqualitätssensor kommuniziert über den i2c bus.

**Gehäuse**
- **Design**: TinkerCad.com
- **Download (.stl Format)**:
https://www.thingiverse.com/thing:4644826
- **Material**:   PLA (Sunlu), Holzoptik, carbonschwarz, transparent
- **Temperatur**: 195°C Noozle / Heizbett 58°
- **Einstellungen**: keine Stützstrukturen
- **Slicer**: Cura 4.8.0

**Bauanleitung**

Eine ***Bauanleitung*** mit allen Angaben von Druck der Bauteile über Verkabelung bis Programmierung befindet sich ***im Github Wiki***:

- https://github.com/pan0ne/CO2-Ampel/wiki/Home

[Weitere Fotos in der Galerie auf Thingiverse](https://www.thingiverse.com/thing:4644826)
