/***************************************************************************
                          ESP8266 Wifi Controller
                   Voreinstellungen und Verbindungsdaten
 ****************************************************************************/
#define SECRET_SSID "MySSID"    // SSID des WLAN
#define SECRET_PASS "MyPassword"  // PSW 

#define SECRET_CH_ID 000000     // thingspeak channel number
#define SECRET_WRITE_APIKEY "XYZ"   // thingspeak channel write API Key

/***************************************************************************
                         ESP32 HelTec LoRa Controller
    Go to your TTN console register a device then the copy fields
 ****************************************************************************/
const char* devEui = "xxxxxxx"; // Change to TTN Device EUI
const char* appEui = "xxxxxxx"; // Change to TTN Application EUI
const char* appKey = "xxxxxxx"; // Chaneg to TTN Application Key
#define BAND    868E6  //you can set band here directly,e.g. 868E6,915E6

/***************************************************************************
                          CO2 Sensor - Einstellungen  
 Die Kallibrierung sollte immer an der frischen Luft stattfinden. 
 Zuerst 10 Minuten lang stehen lassen und dann Knopf drücken.                         
 ****************************************************************************/
#define RX_PIN 13       // Rx pin (D7) which the MHZ19 Tx pin is attached to
#define TX_PIN 12       // Tx pin (D8) which the MHZ19 Rx pin is attached to
#define BAUDRATE 9600   // Device to MH-Z19 Serial baudrate (should not be changed)
#define MHZ_RANGE 5000  // Sensibilität der Messungen, bis 2000ppm, bis 5000ppm, bis 10000ppm
#define FlashButtonPIN 0 // Der Flash Button des NodeMCU Amica kallibriert die C02 Ampel und setzt den Wert zurück auf 400ppm. 
volatile int dMode = 1;
int last_dMode = 0;


/***************************************************************************
                          Temperaturwertkorrektur 
Temperaturwert des BME680 korrigieren. 
Der Wert kann positiv und negativ sein. Zum Beispiel 2 oder -2                        
 ****************************************************************************/
int temp_adjust = 0;

/***************************************************************************
                Neopixel - Anzahl, Pin, Helligkeit
****************************************************************************/
#define PIN       2     // Pin - auf dem Heltec LoRa Wifi v2 ist es Pin 25
#define NUMPIXELS 4     // Anzahl der Pixel
