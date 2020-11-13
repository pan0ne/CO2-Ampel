/*
  CO2 Ampel
  Dies ist ein Beispiel Arduino Sketch für eine CO2 Ampel. 
  
  Notice:
  The onboard OLED display is SSD1306 driver and I2C interface. In order to make the
  OLED correctly operation, you should output a high-low-high(1-0-1) signal by soft-
  ware to OLED's reset pin, the low-level signal at least 5ms.

  OLED Pins -> ESP32:
  OLED_SDA -- GPIO4
  OLED_SCL -- GPIO15
  OLED_RST -- GPIO16
  
  MH-Z19b Pins -> ESP32:
  MH-Z19b_RX - TX Pin (GPIO 1)
  MH-Z19b_TX - RX Pin (GPIO 3) 
 
  Neopixel Pin -> ESP32:
  Di - Pin 25 (GPIO 25)
  
  BME680 Pins -> ESP32:
  SDA - Pin 21 (GPIO 21)
  SCL - Pin 22 (GPIO 22)
  
  Touch Button Pin -> ESP32:
  Di - Pin 13 (GPIO 13)  
  
  */
#include <Arduino.h>
#include <TTN_esp32.h>
#include <Adafruit_NeoPixel.h>
#include "TTN_CayenneLPP.h"
#include "heltec.h"
#include "images.h"
#include "MHZ19.h"
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

/***************************************************************************
    CO2 Sensor
 ****************************************************************************/
unsigned long getDataTimer = 0;
#define RX_PIN 3                                         // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN 1                                         // Tx pin which the MHZ19 Rx pin is attached to
#define BAUDRATE 9600                                      // Device to MH-Z19 Serial baudrate (should not be changed)
MHZ19 myMHZ19;                                             // Constructor for library
//SoftwareSerial Serial2(RX_PIN, TX_PIN);                   // (Uno example) create device to MH-Z19 serial
HardwareSerial mySerial(1); 
int8_t Temp;
int CO2;


/***************************************************************************
    BME 680 Sensor
 ****************************************************************************/

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme; // I2C

float hum_weighting = 0.25; // so hum effect is 25% of the total air quality score
float gas_weighting = 0.75; // so gas effect is 75% of the total air quality score

int   humidity_score, gas_score;
float gas_reference = 2500;
float hum_reference = 40;
int   getgasreference_count = 0;
int   gas_lower_limit = 10000;  // Bad air quality limit
int   gas_upper_limit = 300000; // Good air quality limit

/***************************************************************************
    Go to your TTN console register a device then the copy fields
    and replace the CHANGE_ME strings below
 ****************************************************************************/
const char* devEui = "xxx"; // Change to TTN Device EUI
const char* appEui = "xxx"; // Change to TTN Application EUI
const char* appKey = "xxx"; // Chaneg to TTN Application Key
#define BAND    868E6  //you can set band here directly,e.g. 868E6,915E6

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
    // Serial.write(payload[i]);
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
  Heltec.display->drawXbm(85, 2, logo_width, logo_height, logo_bits);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(15, 15, "DUISentrieb");
  delay(1000);
  Heltec.display->setFont(ArialMT_Plain_24);
  Heltec.display->drawString(2, 35, "CO² Ampel");
  Heltec.display->display();
  delay(6000);
}

/***************************************************************************
    Neopixel

 ****************************************************************************/

#define PIN        25 // Pin - auf dem Heltec LoRa Wifi v2 ist es Pin 25
#define NUMPIXELS 6 // Anzahl der Pixel
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {

  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
    for (int i = 0; i < pixels.numPixels(); i++) { // For each pixel in strip...
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / pixels.numPixels());
      pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHue)));
    }
    pixels.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < pixels.numPixels(); i++) { // For each pixel in strip...
    pixels.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    pixels.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}



/***************************************************************************
    Setup

 ****************************************************************************/
void setup()
{
   Wire.begin();
    if (!bme.begin()) {
      Serial.println("Could not find a valid BME680 sensor, check wiring!");
      while (1);
    } else Serial.println("Found a sensor");

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320°C for 150 ms
  // Now run the sensor to normalise the readings, then use combination of relative humidity and gas resistance to estimate indoor air quality as a percentage.
  // The sensor takes ~30-mins to fully stabilise
  GetGasReference();

  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  Serial.begin(115200);
  colorWipe(pixels.Color(  0, 150,   150), 40); // Blue
  Heltec.display->init();
  Heltec.display->flipScreenVertically();
  logo();
  colorWipe(pixels.Color(  255, 71,   0), 40); // Orange

  Heltec.display->clear();
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(25, 10, "LoRa TTN");
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(5, 30, "Verbindungsversuch...");
  Heltec.display->display();
  delay(3000);
  ttn.begin();
  ttn.onMessage(message);
  ttn.join(devEui, appEui, appKey);
  while (!ttn.isJoined())
  {
    Heltec.display->clear();
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(20, 20, "Verbinde zu TTN");
    Heltec.display->drawString(30, 40, "Bitte warten...");
    Heltec.display->display();
    rainbow(5);
    delay(1000);
  }
  colorWipe(pixels.Color(  0, 150,   0), 70); // Green
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


void loop()
{
  Serial.println(touchRead(T4));  // get value using T4 Touch Sensor
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
  co2Warnung();

  if (touchRead(T4) == 1){
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(5, 2, " Temperature = " + String(bme.readTemperature(), 2)     + "°C");
  Heltec.display->drawString(5, 12, " Pressure = " + String(bme.readPressure() / 100.0F) + " hPa");
  Heltec.display->drawString(5, 22, " Humidity = " + String(bme.readHumidity(), 1)        + "%");
  Heltec.display->drawString(5, 33, " Gas = " + String(gas_reference)               + " ohms\n");
 
  humidity_score = GetHumidityScore();
  gas_score      = GetGasScore();

  //Combine results for the final IAQ index value (0-100% where 100% is good quality air)
  float air_quality_score = humidity_score + gas_score;
  if ((getgasreference_count++) % 5 == 0) GetGasReference();
 Heltec.display->display();
 delay(3000);   
  };  // get value using T4 Touch Sensor

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
  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  delay(2000); 
  digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  delay(2000);                       // wait for a second
}

void readMHZ19b()
{
  const int INTERVAL = 2000;
  static unsigned long previousMillis;
  static int counter;

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

    //counter++;
    
     if (counter > 10){
       counter = 0;
      }

      return;
  }
}
void co2Warnung()
{
        if(CO2 < 850) {
          colorWipe(pixels.Color(  0, 255,   0), 150); // Green
        } else {
        if(CO2 < 1400) {
          colorWipe(pixels.Color(  150, 150,   0), 150); // Yellow
        } else {         
        if(CO2 < 1800) {
           colorWipe(pixels.Color(  255, 71,   0), 40); // Orange
        } else {
            colorWipe(pixels.Color(255,   0,   0), 150); // Red
            }
          }
        }
}

void GetGasReference() {
  // Now run the sensor for a burn-in period, then use combination of relative humidity and gas resistance to estimate indoor air quality as a percentage.
  //Serial.println("Getting a new gas reference value");
  int readings = 10;
  for (int i = 1; i <= readings; i++) { // read gas for 10 x 0.150mS = 1.5secs
    gas_reference += bme.readGas();
  }
  gas_reference = gas_reference / readings;
  //Serial.println("Gas Reference = "+String(gas_reference,3));
}

String CalculateIAQ(int score) {
  String IAQ_text = "air is ";
  score = (100 - score) * 5;
  if      (score >= 301)                  IAQ_text += "Hazardous";
  else if (score >= 201 && score <= 300 ) IAQ_text += "Very Unhealthy";
  else if (score >= 176 && score <= 200 ) IAQ_text += "Unhealthy";
  else if (score >= 151 && score <= 175 ) IAQ_text += "Unhealthy for Sensitive Groups";
  else if (score >=  51 && score <= 150 ) IAQ_text += "Moderate";
  else if (score >=  00 && score <=  50 ) IAQ_text += "Good";
  Heltec.display->drawString(5, 44, "IAQ Score = " + String(score) + ", ");
  return IAQ_text;
}

int GetHumidityScore() {  //Calculate humidity contribution to IAQ index
  float current_humidity = bme.readHumidity();
  if (current_humidity >= 38 && current_humidity <= 42) // Humidity +/-5% around optimum
    humidity_score = 0.25 * 100;
  else
  { // Humidity is sub-optimal
    if (current_humidity < 38)
      humidity_score = 0.25 / hum_reference * current_humidity * 100;
    else
    {
      humidity_score = ((-0.25 / (100 - hum_reference) * current_humidity) + 0.416666) * 100;
    }
  }
  return humidity_score;
}

int GetGasScore() {
  //Calculate gas contribution to IAQ index
  gas_score = (0.75 / (gas_upper_limit - gas_lower_limit) * gas_reference - (gas_lower_limit * (0.75 / (gas_upper_limit - gas_lower_limit)))) * 100.00;
  if (gas_score > 75) gas_score = 75; // Sometimes gas readings can go outside of expected scale maximum
  if (gas_score <  0) gas_score = 0;  // Sometimes gas readings can go outside of expected scale minimum
  return gas_score;
}
