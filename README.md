***CO2-Messgerät/Ampel***

Dieses Projekt zeigt den Bau einer CO2 Ampel.
Benutzt wird ein **ESP8266 / ESP32** Mikrocontroller und ein **MHZ-19b CO2 Sensor**. Zusätzlich kann ein **BME680 Luftqualitätssensor** angeschlossen werden um die Messwerte zu präzisieren sowie**Neopixel** und **SSD1306 Display** zur visuellen Ausgabe.  

***Die CO2 Ampel erfasst:***
- **Temperatur (C)**
- **Luftfeuchte (%)**
- **CO2 (ppm)**,
- **Luftdruck(hPa)**
- **Luftqualität (VOC, flüchtige organische Verbindungen)**.

Die **Sensordaten** können via **LoRa WAN** an das **thethingsnetwork.org** gesendet werden, oder direkt per **WiFi**.
Zur **Analyse der Daten** werden diese an **thingsspeak.com** übertragen und grafisch aufbereitet. (Account nötig).
Bis auf den CO2 Sensor, der mit 5V betrieben wird, reicht für alles andere 3.3V. Der CO2 Sensor wird seriell über die RX/TX Pins abgefragt. Der Luftqualitätssensor kommuniziert über den i2c bus.

**3D Druckmodelle**
- **Design**: TinkerCad.com
- **Download (.stl Format)**:
https://www.thingiverse.com/thing:4644826
- **Material**:   PLA (Sunlu), Holzoptik, carbonschwarz, transparent
- **Temperatur**: 195°C Noozle / Heizbett 58°
- **Einstellungen**: keine Stützstrukturen
- **Slicer**: Cura 4.8.0

Eine ***Bauanleitung*** mit allen Angaben von Druck der Bauteile über Verkabelung bis Programmierung befindet sich ***im Github Wiki***:

- https://github.com/pan0ne/CO2-Ampel/wiki/Anleitung

**Videoanleitung**

[![YouTube Anleitung](http://img.youtube.com/vi/UA3pel5LR24/0.jpg)](http://www.youtube.com/watch?v=UA3pel5LR24 "DIY CO2 Ampel")

[Weitere Fotos in der Galerie auf Thingiverse](https://www.thingiverse.com/thing:4644826)
