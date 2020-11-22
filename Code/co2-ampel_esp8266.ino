/*
  CO2 Ampel
  Dies ist ein Beispiel Arduino Sketch für eine CO2 Ampel 
  mit einem NodeMCU ESP8266 Amica und SSD1306 Display

  OLED Pins -> ESP8266_amica:
  OLED_SDA -- D1
  OLED_SCL -- D2
  
  MH-Z19b Pins -> ESP8266_amica:
  MH-Z19b_RX - TX Pin
  MH-Z19b_TX - RX Pin 
 
  Neopixel Pin -> ESP8266_amica:
  Di - D5 (GPIO 25)
  
  BME680 Pins -> ESP8266_amica:
  SDA - D2 (GPIO 2)
  SCL - D1 (GPIO 1)
  
  Touch Button Pin -> ESP8266:
  Di - D10 (GPIO 10)  
  
  */
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
#include "images.h"
#include "config.h"
#include "MHZ19.h"
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include "ThingSpeak.h"
#include <ESP8266WiFi.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


/***************************************************************************
    WiFi und thingspeak
 ****************************************************************************/
char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

String myStatus = "";

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
    Logo aus der images.h
    Duisentrieb Logo und Schriftzug
 ****************************************************************************/
void logo()
{
  display.clearDisplay();
  display.drawBitmap(
    (display.width()  - logo_width ) / 2,
    (display.height() - logo_height) / 2,
    co2a_logo, logo_height, logo_height, 1);
  display.setTextSize(10);
    display.setCursor(15,15);
  display.println("DUISentrieb");
  delay(1000);
  display.setTextSize(24);
    display.setCursor(2,35);
  display.println("CO² Ampel");
  display.display();
  delay(6000);
}

/***************************************************************************
    Neopixel

 ****************************************************************************/

#define PIN       5 // Pin - auf dem Heltec LoRa Wifi v2 ist es Pin 25
#define NUMPIXELS 8 // Anzahl der Pixel
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
    Touch Button

 ****************************************************************************/

#define T4  10 // GPIO 10

/***************************************************************************
    Setup

 ****************************************************************************/
void setup()
{
  
  WiFi.mode(WIFI_STA); 
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

  Serial.begin(115200);
  colorWipe(pixels.Color(  0, 150,   150), 40); // Blue
   // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.clearDisplay();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  logo();
  colorWipe(pixels.Color(  255, 71,   0), 40); // Orange

  ThingSpeak.begin(client);  // Initialize ThingSpeak
  colorWipe(pixels.Color(  0, 150,   0), 70); // Green
  display.clearDisplay();
  display.setTextSize(16);
    display.setCursor(5,30);
  display.println("Verbunden");
  display.display();
  delay(3000);
  logo();

SoftwareSerial mySerial(RX_PIN, TX_PIN);                   // (Uno example) create device to MH-Z19 serial
  myMHZ19.begin(mySerial);                                // *Serial(Stream) refence must be passed to library begin(). 
  myMHZ19.autoCalibration();                          // Turn auto calibration ON (OFF autoCalibration(false))

}


void loop()
{
  
  Serial.println(digitalRead(T4));  // get value using T4 Touch Sensor
  display.clearDisplay();
  display.setTextSize(24);
      display.setCursor(60,2);
  display.println("CO2");
  display.setTextSize(16);
        display.setCursor(120,40);
  display.println(" ppm");
  display.display();
  display.setTextSize(24);
  readMHZ19b();
  display.setCursor(10,30);
  display.println(String(CO2));
  display.display();
  co2Warnung();

  if (digitalRead(T4) == 1){
  display.clearDisplay();
  display.setTextSize(10);
    display.setCursor(5,2);
  display.println(" Temperature = " + String(bme.readTemperature(), 2)     + "°C");
    display.setCursor(5,12);
  display.println(" Pressure = " + String(bme.readPressure() / 100.0F) + " hPa");
    display.setCursor(5,22);
  display.println(" Humidity = " + String(bme.readHumidity(), 1)        + "%");
    display.setCursor(5,33);
  display.println(" Gas = " + String(gas_reference)          + " ohms\n");
 
  humidity_score = GetHumidityScore();
  gas_score      = GetGasScore();

  //Combine results for the final IAQ index value (0-100% where 100% is good quality air)
  float air_quality_score = humidity_score + gas_score;
  if ((getgasreference_count++) % 5 == 0) GetGasReference();
 display.display();
 delay(3000);   
  };  // get value using T4 Touch Sensor

  // set the fields with the values
  ThingSpeak.setField(1, CO2);
  ThingSpeak.setField(2, bme.readTemperature());
  ThingSpeak.setField(3, bme.readHumidity());
  ThingSpeak.setField(4, gas_score );
  
  // set the status
  ThingSpeak.setStatus(myStatus);
  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
    myStatus = String("Online");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
    myStatus = String("Offline");
  }

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
  //display.write(5, 44, "IAQ Score = " + String(score) + ", ");
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