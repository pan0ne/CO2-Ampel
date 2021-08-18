/*
  ESP8266 CO2 Ampel
  Author: Pan0ne
  Date/Version: 202108122146
  Libraries - Boardmanager:
    "TTN_ESP32" https://github.com/rgot-org/TheThingsNetwork_esp32
    "Adfruit NeoPixel" https://github.com/adafruit/Adafruit_NeoPixel
    "HelTec" https://github.com/HelTecAutomation/Heltec_ESP32 -
    "MHZ19" https://github.com/tobiasschuerg/MH-Z-CO2-Sensors
    "Adafruit_BME680" und "Adafruit_Sensors"
    "ESP SoftwareSerial" https://github.com/plerup/espsoftwareserial/
    Please check config.h for configurations
    #if DATA_LOG_INFLUX
    #include <ESP8266Influxdb.h>  // http://librarymanager#ESP8266_InfluxDB
    #endif
*/

#include <Arduino.h>            // https://github.com/esp8266/Arduino

#ifdef __AVR__
  #include <avr/power.h>
#endif

#include <ArduinoJson.h>        // https://github.com/bblanchon/ArduinoJson
#include <Wire.h>               // http://librarymanager#Wire
#include "images.h"             // Create new tab in Arduino IDE and copy example from github if not there
#include "config.h"             // Create new tab in Arduino IDE and copy example from github if not there
#include <SoftwareSerial.h>     // http://librarymanager#SoftSerial
#include <Adafruit_NeoPixel.h>  // http://librarymanager#Adafruit_NeoPixel
#include "SSD1306Wire.h"        // http://librarymanager#SSD1306_esp8266_oled_driver
#include <Adafruit_Sensor.h>    // http://librarymanager#Adafruit_Sensor

#if SENSOR_MHZ19B
  #include "MHZ19.h"            // http://librarymanager#MH-Z19 (by J. Dempsey)
#endif

#if SENSOR_BME680
  #include "Adafruit_BME680.h"  // http://librarymanager#Adafruit_BME680
#endif

/***************************************************************************
  SSD1606 Display OLED Pins -> ESP8266_amica
  OLED_SDA -- D2
  OLED_SCL -- D1
  OLED_RST -- RST
 ****************************************************************************/
SSD1306Wire display(0x3c, SDA, SCL);
/***************************************************************************
  CO2 Sensor -   MH-Z19b Pins -> ESP8266_amica
  MH-Z19b_RX - D6 Pin
  MH-Z19b_TX - D7 Pin
 ****************************************************************************/
unsigned long getDataTimer = 0;
MHZ19 myMHZ19;          // Constructor for library
SoftwareSerial mySerial(RX_PIN, TX_PIN);                   // (Uno example) create device to MH-Z19 serial
//HardwareSerial mySerial(1);
int8_t Temp;
int CO2;
/***************************************************************************
 Button handler (FlashButton als normalen Inputbutton verwenden)
 ****************************************************************************/

void IRAM_ATTR handleInterrupt() {
  dMode++;
}
/***************************************************************************
  BME 680 Sensor / Pins -> ESP8266_amica
  SDA - D2 (GPIO 2)
  SCL - D1 (GPIO 1) 
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
String  air_quality;
/***************************************************************************
    Logo "co2a_logo" -> images.h
    Duisentrieb Logo und Schriftzug
 ****************************************************************************/
void logo()
{
  display.clear();
  display.drawXbm(85, 2, co2a_logo_width, co2a_logo_height, co2a_logo);
  display.setFont(ArialMT_Plain_10);
  display.drawString(15, 15, "DUISentrieb");
  delay(1000);
  display.setFont(ArialMT_Plain_24);
  display.drawString(2, 35, "CO2 Meter");
  display.display();
  delay(6000);
}
/***************************************************************************  
  Neopixel / Pin -> ESP8266_amica
  Di - D5 (GPIO 25)
 ****************************************************************************/
Adafruit_NeoPixel pixels(atof(pixel), PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  //pixels.setBrightness(atof(pixel_brightness));
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
  pixels.setBrightness(atof(pixel_brightness));
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
  Serial.begin(9600);
  
  #if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  
  pixels.begin();
  pixels.show();  // Turn OFF all pixels ASAP
  pixels.setBrightness(100);
  rainbow(40);  //colorWipe(pixels.Color(255,   0,   0), 150); // Red

  Wire.begin();
  bme.begin();

    if (!bme.begin()) {
      #if DEBUG
      Serial.println("Could not find a valid BME680 sensor, check wiring!");
      #endif
    } else {
      #if DEBUG
      Serial.println("Found a sensor");
      #endif
    }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
  delay(500);
  bme.performReading();
  delay(500);
  GetGasReference();

  //SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally

  // Initialising the UI will init the display too.
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.clear();
  delay(2000); // Pause for 2 seconds
  display.clear(); // Clear the buffer

  logo();

  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.drawString(60, 10, "CO2 Meter");  
  display.drawString(60, 35, "Offline Mode");
  display.display();

  mySerial.begin(BAUDRATE);          // Uno Example: Begin Stream with MHZ19 baudrate
  myMHZ19.begin(mySerial);
  myMHZ19.autoCalibration(MHZ_CLB_MODE);   // Turn auto calibration ON (OFF autoCalibration(false))
 
  pinMode(FlashButtonPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FlashButtonPIN), handleInterrupt, FALLING);

}


void loop()
{
  readMHZ19b();
  humidity_score = GetHumidityScore();
  gas_score      = GetGasScore();
  //Combine results for the final IAQ index value (0-100% where 100% is good quality air)
  float air_quality_score = humidity_score + gas_score;
  if ((getgasreference_count++) % 5 == 0) GetGasReference();
  air_quality = CalculateIAQ(air_quality_score);  
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);
  display.drawString(62, 1, String(air_quality));
  display.drawXbm(12, 14, co2_sym_width, co2_sym_height, co2_sym);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_24);
  display.drawString(50, 15, String(CO2));
  display.setFont(ArialMT_Plain_10);
  display.drawString(100, 27," ppm" );
  display.setFont(ArialMT_Plain_10);
  display.drawXbm(2, 46, temperature_icon_width, temperature_icon_height, temperature_icon);
  display.drawString(20, 48, String(bme.readTemperature() + temp_adjust, 1)  + " °C");
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.setFont(ArialMT_Plain_10);
  //display.drawString(120, 50, String(Temp) + "°C");
  display.drawXbm(80, 46, humidity_icon_width, humidity_icon_height, humidity_icon);
  display.drawString(128, 48, String(bme.readHumidity(), 1)  + " %");
  display.display();
  co2Warnung();
   if (dMode != last_dMode) { 
            display.clear();
            display.setTextAlignment(TEXT_ALIGN_CENTER);
            display.setFont(ArialMT_Plain_24);
            display.drawString(60, 20, "Kalibrierung \n gestartet");
            delay(1000);  
            display.clear();  
            display.setFont(ArialMT_Plain_24);
            display.drawString(60, 1, "Achtung!");
            display.setFont(ArialMT_Plain_10);
            display.drawString(60, 25, "Gerät sollte vorher \n min. 10 Minuten an der \n frischen Luft stehen!");
            display.display(); 
            myMHZ19.calibrateZero();
            delay(5000);
            display.clear();
            display.drawString(60, 20, "Kalibrierung \n erfolgreich!");
            display.display();
            delay(1000);    
            last_dMode = dMode;
     }
}

void readMHZ19b()
{
  const int INTERVAL = 3000;
  static unsigned long previousMillis;
  static int counter;

  if (millis() - previousMillis > INTERVAL)
  {
    previousMillis = millis();
    CO2 = myMHZ19.getCO2();           // Request CO2 (as ppm)
    Temp = myMHZ19.getTemperature();   // Request Temperature (as Celsius)
    #if DEBUG
    Serial.print(F("CO2 und Temperatur lesen: "));
    Serial.println(counter);    
    Serial.print(F("CO2: "));
    Serial.println(CO2);    
    Serial.print(F("MHZ Temp: "));
    Serial.println(Temp);
    #endif 
    counter++;
     if (counter > 10){
       counter = 0;
      }
      return;
  }
}

/*  Neopixel colored by CO2 Level
 *  CO2 < 850ppm -> color is green 
 *  over 850ppm to 1200ppm -> color is yellow
 *  above 1600 ppm orange, then red or rainbow
 */
void co2Warnung()
{
  if(CO2 < 850) {
    colorWipe(pixels.Color(  0, 255,   0), 30); // green
  }else{
  if(CO2 < 1200) {
    colorWipe(pixels.Color(  150, 150,   0), 70); // yellow
  }else{
  if(CO2 < 1600) {
     colorWipe(pixels.Color(  255, 71,   0), 100); // orange
  }else{    
      rainbow(10);  // Rainbox
      //colorWipe(pixels.Color(255,   0,   0), 150); // Red
      }
  }
  }
}

void co2WarnungTone()
{
  if(CO2 < 1500) {
     if((CO2 > 1400) && (CO2 < 1500 )) {
      tone(14, 800); // …spiele diesen Ton...
      delay(100); //…und zwar für eine Sekunde...
      tone(14, 600); // …spiele diesen Ton...
      delay(200); //…und zwar für eine Sekunde...
      tone(14, 400); // …spiele diesen Ton...
      delay(300); //…und zwar für eine Sekunde...
      noTone(14); // Ton abschalten
      }
  } else {
  if(CO2 < 2000) {
      if((CO2 > 1900) && (CO2 < 2000 )) {
      tone(14, 800); // …spiele diesen Ton...
      delay(100); //…und zwar für eine Sekunde...
      tone(14, 600); // …spiele diesen Ton...
      delay(200); //…und zwar für eine Sekunde...
      tone(14, 400); // …spiele diesen Ton...
      delay(300); //…und zwar für eine Sekunde...
      noTone(14); // Ton abschalten
      }
  } else {
      tone(14, 600); // …spiele diesen Ton...
      delay(200); //…und zwar für eine Sekunde...
      noTone(14); // Ton abschalten
      }
    }      
}

void GetGasReference() {
  // Now run the sensor for a burn-in period, then use combination of relative humidity and gas resistance to estimate indoor air quality as a percentage.
  #if DEBUG
  Serial.println("Getting a new gas reference value");
  #endif
  int readings = 10;
  for (int i = 1; i <= readings; i++) { // read gas for 10 x 0.150mS = 1.5secs
    gas_reference += bme.readGas();
  }
  gas_reference = gas_reference / readings;
  #if DEBUG
  Serial.println("Gas Reference = "+String(gas_reference,3));
  #endif
}

String CalculateIAQ(int score) {
  String IAQ_text = "Luft ";
  score = (100 - score) * 5;
  if      (score >= 651)                  IAQ_text += "ist nicht gut (Stufe 5)";
  else if (score >= 451 && score <= 650 ) IAQ_text += "ist schlecht (Stufe 4)";
  else if (score >= 251 && score <= 450 ) IAQ_text += "noch OK (Stufe 3)";
  else if (score >= 151 && score <= 250 ) IAQ_text += "ist gut (Stufe 2)";
  else if (score >=  00 && score <= 150 ) IAQ_text += "ist frisch (Stufe 1)";
  //display.setTextAlignment(TEXT_ALIGN_CENTER);
  //display.setFont(ArialMT_Plain_10);
  //display.drawString(60, 59, "IAQ Score = " + String(score) + ", ");
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

float getBatteryVoltage(){
    //************ Measuring Battery Voltage Example ***********
    float sample1 = 0;
    // Get 100 analog read to prevent unusefully read
    for (int i = 0; i < 100; i++) {
        sample1 = sample1 + analogRead(A0); //read the voltage from the divider circuit
        delay(2);
    }
    sample1 = sample1 / 100;
    // REFERENCE_VCC is reference voltage of microcontroller 3.3v for esp8266 5v Arduino
    // BAT_RES_VALUE_VCC is the kohom value of R1 resistor
    // BAT_RES_VALUE_GND is the kohom value of R2 resistor
    // 1023 is the max digital value of analog read (1024 == Reference voltage)
    float batVolt = (sample1 * REFERENCE_VCC  * (BAT_RES_VALUE_VCC + BAT_RES_VALUE_GND) / BAT_RES_VALUE_GND) / 1023;
    return batVolt;
}
