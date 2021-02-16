/*
  CO2 Meter (Heltec WiFi LoRa v2 + MHZ-19B CO2 Sensor)
*/
#include <Arduino.h>
#include <TTN_esp32.h>
#include "TTN_CayenneLPP.h"
#include "heltec.h" 
#include "images.h"
#include "config.h"
#include "MHZ19.h"

/***************************************************************************
    CO2 Sensor
 ****************************************************************************/
 
unsigned long getDataTimer = 0;
#define RX_PIN 3                                         // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN 1                                         // Tx pin which the MHZ19 Rx pin is attached to
#define BAUDRATE 9600                                    // Device to MH-Z19 Serial baudrate (should not be changed)
MHZ19 myMHZ19;                                           // Constructor for library
HardwareSerial mySerial(1); 
int8_t Temp;
int CO2;

/***************************************************************************
  LoRa (Verbindungsdaten und Bandinfo liegen in config.h)
 ****************************************************************************/

unsigned int counter = 0;
String rssi = "RSSI --";
String packSize = "--";
String packet ;

TTN_esp32 ttn ;
TTN_CayenneLPP lpp;

/***************************************************************************
  Debug LORA Info
 ****************************************************************************/
void message(const uint8_t* payload, size_t size, int rssi)
{
  Serial.println("-- MESSAGE");
  Serial.print("Received " + String(size) + " bytes RSSI=" + String(rssi) + "db");
  for (int i = 0; i < size; i++)
  {
    Serial.print(" " + String(payload[i]));
    Serial.write(payload[i]);
  }

  Serial.println();
}

/***************************************************************************
    Logo aus der images.h
    Duisentrieb Logo und Schriftzug
 ****************************************************************************/
 
void logo()
{
  Heltec.display->clear();
  Heltec.display->drawXbm(85, 2, logo_width, logo_height, co2a_logo);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(15, 15, "DUISentrieb");
  delay(1000);
  Heltec.display->setFont(ArialMT_Plain_24);
  Heltec.display->drawString(2, 35, "CO2 Ampel");
  Heltec.display->display();
  delay(6000);
}

/***************************************************************************
                                      Setup
 ****************************************************************************/
 
void setup()
{
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  Serial.begin(115200);
  Heltec.display->init();
  Heltec.display->flipScreenVertically();
  logo();

  Heltec.display->clear();
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(25, 10, "LoRa TTN");
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(5, 30, "Verbindungsversuch...");
  Heltec.display->display();
  delay(3000);
  
  if (!ttn.begin()){
    ttn.onMessage(message);
    ttn.join(devEui, appEui, appKey);
    while (true);
  }
  
  while (!ttn.isJoined() && millis() < 10000)
  {
    ttn.showStatus();
    Heltec.display->clear();
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(20, 20, "Verbinde zu TTN");
    Heltec.display->drawString(30, 40, "Bitte warten...");
    Heltec.display->display();
    delay(1000);
  }

  //Serial.println("LoRa init succeeded.");
  
  Heltec.display->clear();
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(5, 30, "TTN verbunden");
  Heltec.display->display();
  delay(3000);
  
  logo();
  
  ttn.showStatus();

  mySerial.begin(BAUDRATE, SERIAL_8N1, RX_PIN, TX_PIN); // (ESP32 Example) device to MH-Z19 serial start   
  myMHZ19.begin(mySerial);                                // *Serial(Stream) refence must be passed to library begin(). 
  myMHZ19.autoCalibration();                          // Turn auto calibration ON (OFF autoCalibration(false))

}

/***************************************************************************
                                      Loop
 ****************************************************************************/
void loop()
{
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_CENTER);
  Heltec.display->setFont(ArialMT_Plain_24);
  Heltec.display->drawString(60, 0, "CO2");
  Heltec.display->setTextAlignment(TEXT_ALIGN_RIGHT);
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(120, 40, " ppm");
  Heltec.display->display();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_24);
  readMHZ19b();
  Heltec.display->drawString(10, 30, String(CO2));
  Heltec.display->display();
 
 delay(3000);   

  // send LoRa Packets
    // Split word (16 bits) into 2 bytes of 8
    byte payload[4];
    payload[0] = highByte(CO2);
    payload[1] = lowByte(CO2);
    payload[2] = highByte(Temp);
    payload[3] = lowByte(Temp);

    if (ttn.sendBytes(payload, 4))
      {
          Serial.printf("CO2: %d ppm - TTN_CayenneLPP: %02X%02X\n", CO2, payload[0], payload[1]);
          Serial.printf("Temperatur: %d C° - TTN_CayenneLPP: %02X%02X\n", Temp, payload[2], payload[3]);
      }

  counter++;

  /*  
  Notice:
  The onboard OLED display is SSD1306 driver and I2C interface. In order to make the
  OLED correctly operation, you should output a high-low-high(1-0-1) signal by soft-
  ware to OLED's reset pin, the low-level signal at least 5ms.
   */
  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  delay(2000); 
  digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  delay(2000);                       // wait for a second
}


/***************************************************************************
                                      Sensor
 ****************************************************************************/
void readMHZ19b()
{
  const int INTERVAL = 2000;
  static unsigned long previousMillis;

  if (millis() - previousMillis > INTERVAL)
  {
    previousMillis = millis();

    Serial.print(F("CO2 und Temperatur lesen: "));  
    Serial.println(counter);
    
    CO2 = myMHZ19.getCO2();           // Request CO2 (as ppm)    
    Serial.print(F("CO2: "));     
    Serial.println(CO2);
    
    Temp = myMHZ19.getTemperature();   // Request Temperature (as Celsius)
    Serial.print(F("Temp: "));
    Serial.println(CO2);      
  }
}
