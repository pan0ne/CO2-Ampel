// Use this file to store all of the private credentials 
// and connection details

#define DEBUG true
#define SENSOR_MHZ19B true 
#define SENSOR_BME680 true
#define SENSOR_BME280 false

// CO2 Sensor - Einstellungen der Pins
#define RX_PIN 13       // Rx pin (D7) which the MHZ19 Tx pin is attached to
#define TX_PIN 12       // Tx pin (D8) which the MHZ19 Rx pin is attached to
#define BAUDRATE 9600   // Device to MH-Z19 Serial baudrate (should not be changed)
#define MHZ_RANGE 5000  // Sensibilität der Messungen, bis 2000ppm, bis 5000ppm, bis 10000ppm
#define MHZ_CLB_MODE

// Der Flash Button des NodeMCU Amica kallibriert die C02 Ampel und setzt den Wert zurück auf 400ppm. 
// Die Kallibrierung sollte immer an der frischen Luft stattfinden. 10 Minuten lang stehen lassen und dann Knopf drücken.
#define FlashButtonPIN 0

// Neopixel - Anzahl der Pixel, Helligkeit, Pin
#define PIN       2 // Pin - auf dem Heltec LoRa Wifi v2 ist es Pin 25

// Battery voltage resistance
#define BAT_RES_VALUE_GND 20.0
#define BAT_RES_VALUE_VCC 10.0
#define REFERENCE_VCC 3.3

//define your default values here, if there are different values in config.json, they are overwritten.
char pixel[3]  = "3";
char pixel_brightness[5] = "200";
char callibration_mode[6] = "false";

// Temperaturwert des BME680 korrigieren. Der Wert kann positiv und negativ sein. Zum Beispiel 2 oder -2
int temp_adjust = 0;

// Init manual calibration mode
int dMode = 0;
int last_dMode = 0;

// set PushButton pin number
//const int BUTTON_PIN = 0;

int buttonState;

unsigned long timePress = 0;
unsigned long timePressLimit = 0;
int clicks = 0;
