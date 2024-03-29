//Convert png to xbm https://convertio.co/ 

#define co2a_logo_width 35
#define co2a_logo_height 35
const unsigned char co2a_logo[] = {
  0x00, 0x00, 0x00, 0x80, 0x07, 0x00, 0x00, 0x00, 0xF8, 0x04, 0x00, 0x00, 
  0x00, 0x06, 0x04, 0x00, 0x00, 0x80, 0x03, 0x04, 0x00, 0x00, 0xC0, 0x00, 
  0x06, 0x00, 0x00, 0x30, 0x00, 0x06, 0x00, 0x00, 0x18, 0x00, 0x02, 0x00, 
  0x00, 0x04, 0x07, 0x02, 0x00, 0x00, 0x82, 0x0F, 0x02, 0x00, 0x00, 0xC1, 
  0x1C, 0x00, 0x00, 0x80, 0x40, 0x18, 0x03, 0x00, 0x7F, 0x40, 0x1C, 0x01, 
  0x80, 0x61, 0xC0, 0x86, 0x01, 0x80, 0x30, 0x80, 0x87, 0x00, 0xC0, 0x10, 
  0x00, 0xC0, 0x00, 0xC0, 0x08, 0x0C, 0x40, 0x00, 0x60, 0x0C, 0x3B, 0x60, 
  0x00, 0x20, 0x84, 0x1D, 0x20, 0x00, 0x20, 0x86, 0x0E, 0x10, 0x00, 0xF0, 
  0xCF, 0x07, 0x08, 0x00, 0xC0, 0x3B, 0x07, 0x04, 0x00, 0xE0, 0xB0, 0x03, 
  0x06, 0x00, 0x78, 0xD8, 0x01, 0x03, 0x00, 0xFC, 0xF9, 0x80, 0x02, 0x00, 
  0xFE, 0xF1, 0x60, 0x02, 0x00, 0xF0, 0x80, 0x39, 0x02, 0x00, 0x78, 0x02, 
  0x0F, 0x02, 0x00, 0xFC, 0x81, 0x03, 0x02, 0x00, 0xFE, 0x89, 0x02, 0x03, 
  0x00, 0xFE, 0x7F, 0xE3, 0x00, 0x00, 0xC7, 0x7F, 0x3F, 0x00, 0x00, 0xE0, 
  0x3D, 0x07, 0x00, 0x00, 0xE0, 0x1D, 0x03, 0x00, 0x00, 0x60, 0x1C, 0x00, 
  0x00, 0x00, 0x20, 0x04, 0x00, 0x00, 0x00, };

#define co2_sym_width 32
#define co2_sym_height 32
const unsigned char co2_sym [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x03, 0x00, 
  0x00, 0xE0, 0x07, 0x00, 0x00, 0xFE, 0x7F, 0x00, 0x00, 0x7F, 0xFC, 0x00, 
  0x80, 0x37, 0xEC, 0x01, 0x80, 0x03, 0xC0, 0x01, 0xC0, 0x03, 0x80, 0x07, 
  0xF0, 0x03, 0x80, 0x0F, 0xF8, 0xBC, 0x07, 0x1E, 0x38, 0xBE, 0x0F, 0x1C, 
  0x18, 0xC6, 0x0C, 0x18, 0x1C, 0xC6, 0x0C, 0x38, 0x1C, 0xC6, 0x6C, 0x38, 
  0x1C, 0xBE, 0xCF, 0x18, 0x38, 0x38, 0xE7, 0x1C, 0x78, 0x00, 0x60, 0x1E, 
  0xF0, 0x43, 0x80, 0x0F, 0xE0, 0x63, 0x00, 0x03, 0x80, 0xFF, 0x85, 0x03, 
  0x00, 0xFF, 0xFF, 0x03, 0x00, 0x9C, 0xFF, 0x01, 0x00, 0x00, 0xF8, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

#define humidity_icon_width 16
#define humidity_icon_height 16
const unsigned char humidity_icon[] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x40, 0x00, 0xC0, 0x00, 
  0xE0, 0x00, 0xE0, 0x00, 0xF0, 0x00, 0xF0, 0x01, 0xF8, 0x03, 0xF8, 0x07, 
  0xF8, 0x07, 0xF8, 0x07, 0xF0, 0x03, 0xE0, 0x01, };


#define temperature_icon_width 16
#define temperature_icon_height 16
const unsigned char temperature_icon[] =
{
  0xC0, 0x00, 0x20, 0x02, 0xA0, 0x00, 0xE0, 0x02, 0xA0, 0x00, 0xA0, 0x02, 
  0xA0, 0x02, 0xA0, 0x00, 0xA0, 0x02, 0xE0, 0x00, 0xD0, 0x05, 0xD0, 0x05, 
  0xD0, 0x05, 0x60, 0x02, 0xE0, 0x03, 0x00, 0x00, };



#define activeSymbol_width 16
#define activeSymbol_height 16
const unsigned char activeSymbol[] PROGMEM = {
    B00000000,
    B00000000,
    B00011000,
    B00100100,
    B01000010,
    B01000010,
    B00100100,
    B00011000
};


#define inactiveSymbol_width 16
#define inactiveSymbol_height 16
const unsigned char inactiveSymbol[] PROGMEM = {
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00011000,
    B00000000,
    B00000000
};

//Added by Sloeber 
#pragma once
