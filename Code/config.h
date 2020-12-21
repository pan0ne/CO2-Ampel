/***************************************************************************
                    Voreinstellungen und Verbindungsdaten
 ****************************************************************************/
#define SECRET_SSID "MySSID"    // SSID des WLAN
#define SECRET_PASS "MyPassword"  // PSW 

#define SECRET_CH_ID 000000     // thingspeak channel number
#define SECRET_WRITE_APIKEY "XYZ"   // thingspeak channel write API Key

/***************************************************************************
                         HelTec LoRa Controller
    Go to your TTN console register a device then the copy fields
 ****************************************************************************/
const char* devEui = "xxxxxxx"; // Change to TTN Device EUI
const char* appEui = "xxxxxxx"; // Change to TTN Application EUI
const char* appKey = "xxxxxxx"; // Chaneg to TTN Application Key
#define BAND    868E6  //you can set band here directly,e.g. 868E6,915E6
