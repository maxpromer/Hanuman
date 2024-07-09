/*********************************************************************
This is a library for our Monochrome OLEDs based on SSD1309 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

These displays use SPI to communicate, 4 or 5 pins are required to
interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen below must be included in any redistribution
*********************************************************************/

#ifdef __AVR__
  #include <avr/pgmspace.h>
#elif defined(ESP8266) || defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
 #include <pgmspace.h>
#else
 #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif

#if !defined(__ARM_ARCH) && !defined(ENERGIA) && !defined(ESP8266) && !defined(ESP32) && !defined(__arc__)
 #include <util/delay.h>
#endif

#include <stdlib.h>

#include <Wire.h>
//#include <SPI.h>
#include "glcdfont.c"
#include "Adafruit_GFX.h"
#include "OLED_I2C_SSD1309.h"

//-------------------- Condition  test parameter data type for display ---------------//
#ifndef TEST_CHAR_TYPE
#define TEST_CHAR_TYPE(x) *x=='%' && (*(x+1)=='c' || *(x+1)=='C')
#endif

#ifndef TEST_BIN_TYPE
#define TEST_BIN_TYPE(x) *x=='%' && (*(x+1)=='b' || *(x+1)=='B')
#endif

#ifndef TEST_HEX_TYPE
#define TEST_HEX_TYPE(x) *x=='%' && (*(x+1)=='h' || *(x+1)=='H')
#endif

#ifndef TEST_INT_TYPE
#define TEST_INT_TYPE(x) *x=='%' && (*(x+1)=='d' || *(x+1)=='D')
#endif

#ifndef TEST_LONG_TYPE
#define TEST_LONG_TYPE(x) *x=='%' && (*(x+1)=='l' || *(x+1)=='L')
#endif

#ifndef TEST_FLOAT_TYPE
#define TEST_FLOAT_TYPE(x) *x=='%' && (*(x+1)=='f' || *(x+1)=='F')
#endif

unsigned int _floatPrecision=3;
void setFloatPrecision(unsigned int prec)
{
	_floatPrecision=prec;
}

volatile uint _textColor = WHITE;
volatile uint _textBackgroundColor = BLACK;
volatile uint8_t _textSize = 1;
volatile uint8_t _width = 128;
volatile uint8_t _height = 64;	
volatile uint8_t CHAR_PER_ROW = 21;	
volatile uint8_t _rotationMode = 0;	




static uint8_t buffer[SSD1309_LCDHEIGHT * SSD1309_LCDWIDTH / 8] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xe0, 0xf0, 0xfc, 0xbf, 0x3f, 0xfc, 
0xf0, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x02, 0x06, 0x0e, 0x1e, 0x3e, 0x32, 0x32, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 
0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 
0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0xce, 0xe2, 0xf8, 0xfe, 0xff, 0xff, 0x3f, 0x0f, 0x03, 0x03, 0x0f, 
0x3f, 0x3f, 0x0f, 0xc6, 0xf0, 0xfc, 0xfe, 0xfe, 0x7e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 
0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x3e, 0x38, 0x38, 0x3a, 0x3a, 0x3a, 
0x3a, 0x3a, 0x3a, 0x3a, 0x1a, 0x0a, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0xc0, 0xf0, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xf8, 0xf8, 0x78, 0x38, 0x80, 0xe0, 
0xf8, 0x9e, 0xff, 0xff, 0x37, 0x0f, 0xe3, 0xf1, 0xf8, 0xf0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x40, 0x60, 0x70, 0x78, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 
0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x5c, 0x5c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7e, 
0x7f, 0x7f, 0x7f, 0x3f, 0x0f, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x38, 0xfe, 0xff, 0x7f, 
0x1f, 0x0f, 0x03, 0x00, 0x00, 0x00, 0x01, 0x03, 0x0f, 0x1f, 0x7f, 0x7f, 0x7c, 0x70, 0x60, 0x0c, 
0x3c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 
0x7c, 0x5c, 0x5c, 0x7c, 0x70, 0x70, 0x60, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0xe0, 0xf0, 0xf8, 0xf0, 0xc0, 0x00, 0x00, 0x00, 0xf0, 0xf8, 0xf8, 0x38, 0xf0, 
0xf0, 0xe0, 0x00, 0x00, 0x10, 0x38, 0x38, 0xf8, 0xf8, 0x38, 0x38, 0x10, 0x00, 0xf0, 0xf9, 0xf8, 
0x38, 0xf0, 0xf0, 0xc0, 0x00, 0x80, 0xe0, 0xf0, 0x70, 0x38, 0x38, 0x38, 0x70, 0xf0, 0xe0, 0x80, 
0x00, 0xf0, 0xf8, 0xf8, 0xe0, 0xc0, 0x00, 0xf0, 0xf8, 0xf0, 0x00, 0x00, 0x60, 0xa0, 0x20, 0x40, 
0x80, 0x40, 0x20, 0x80, 0x60, 0x30, 0x18, 0x04, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 
0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x18, 0x1f, 0x1f, 0x0f, 0x0e, 0x0f, 0x1f, 0x1e, 0x10, 0x00, 0x1f, 0x1f, 0x1f, 0x07, 0x0f, 
0x1f, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x1f, 
0x07, 0x1f, 0x1f, 0x18, 0x00, 0x03, 0x0f, 0x1f, 0x1c, 0x18, 0x18, 0x18, 0x1c, 0x0f, 0x07, 0x01, 
0x00, 0x1f, 0x1f, 0x1f, 0x01, 0x07, 0x1f, 0x1f, 0x1f, 0x9f, 0x40, 0x20, 0x18, 0x2c, 0x37, 0x18, 
0x18, 0x22, 0x6d, 0x30, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


#define ssd1309_swap(a, b) { int16_t t = a; a = b; b = t; }

// the most basic function, set a single pixel
void OLED_I2C_SSD1309::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    return;

  // check rotation, move pixel around if necessary
  switch (getRotation()) {
  case 1:
    ssd1309_swap(x, y);
    x = WIDTH - x - 1;
    break;
  case 2:
    x = WIDTH - x - 1;
    y = HEIGHT - y - 1;
    break;
  case 3:
    ssd1309_swap(x, y);
    y = HEIGHT - y - 1;
    break;
  }

  // x is which column
    switch (color)
    {
      case WHITE:   buffer[x+ (y/8)*SSD1309_LCDWIDTH] |=  (1 << (y&7)); break;
      case BLACK:   buffer[x+ (y/8)*SSD1309_LCDWIDTH] &= ~(1 << (y&7)); break;
      case INVERSE: buffer[x+ (y/8)*SSD1309_LCDWIDTH] ^=  (1 << (y&7)); break;
    }

}

// initializer for I2C - we only indicate the reset pin!
OLED_I2C_SSD1309::OLED_I2C_SSD1309(int8_t reset) :
Adafruit_GFX(SSD1309_LCDWIDTH, SSD1309_LCDHEIGHT) {
//  sclk = dc = cs = sid = -1;
  rst = reset;
}

void OLED_I2C_SSD1309::begin() {
  this->begin(SSD1309_SWITCHCAPVCC, 0x3C);
  this->show();
}

void OLED_I2C_SSD1309::begin(uint8_t vccstate, uint8_t i2caddr, bool reset) {
  _vccstate = vccstate;
  _i2caddr = i2caddr;

    // I2C Init
//	Wire.setSDA(PB9);
//	Wire.setSCL(PB8);
	Wire.begin();

#ifdef __SAM3X8E__
    // Force 400 KHz I2C, rawr! (Uses pins 20, 21 for SDA, SCL)
    TWI1->TWI_CWGR = 0;
    TWI1->TWI_CWGR = ((VARIANT_MCK / (2 * 400000)) - 4) * 0x101;
#endif
//  }
  // Init sequence
  ssd1309_command(SSD1309_DISPLAYOFF);                    // 0xAE
  ssd1309_command(SSD1309_SETDISPLAYCLOCKDIV);            // 0xD5
  ssd1309_command(0x80);                                  // the suggested ratio 0x80

  ssd1309_command(SSD1309_SETMULTIPLEX);                  // 0xA8
  ssd1309_command(SSD1309_LCDHEIGHT - 1);

  ssd1309_command(SSD1309_SETDISPLAYOFFSET);              // 0xD3
  ssd1309_command(0x0);                                   // no offset
  ssd1309_command(SSD1309_SETSTARTLINE | 0x0);            // line #0
  ssd1309_command(SSD1309_CHARGEPUMP);                    // 0x8D
  if (vccstate == SSD1309_EXTERNALVCC)
    { ssd1309_command(0x10); }
  else
    { ssd1309_command(0x14); }
  ssd1309_command(SSD1309_MEMORYMODE);                    // 0x20
  ssd1309_command(0x00);                                  // 0x0 act like ks0108
  ssd1309_command(SSD1309_SEGREMAP | 0x1);
  ssd1309_command(SSD1309_COMSCANDEC);

 #if defined SSD1309_128_32
  ssd1309_command(SSD1309_SETCOMPINS);                    // 0xDA
  ssd1309_command(0x02);
  ssd1309_command(SSD1309_SETCONTRAST);                   // 0x81
  ssd1309_command(0x8F);

#elif defined SSD1309_128_64
  ssd1309_command(SSD1309_SETCOMPINS);                    // 0xDA
  ssd1309_command(0x12);
  ssd1309_command(SSD1309_SETCONTRAST);                   // 0x81
  if (vccstate == SSD1309_EXTERNALVCC)
    { ssd1309_command(0x9F); }
  else
    { ssd1309_command(0xCF); }

#elif defined SSD1309_96_16
  ssd1309_command(SSD1309_SETCOMPINS);                    // 0xDA
  ssd1309_command(0x2);   //ada x12
  ssd1309_command(SSD1309_SETCONTRAST);                   // 0x81
  if (vccstate == SSD1309_EXTERNALVCC)
    { ssd1309_command(0x10); }
  else
    { ssd1309_command(0xAF); }

#endif

  ssd1309_command(SSD1309_SETPRECHARGE);                  // 0xd9
  if (vccstate == SSD1309_EXTERNALVCC)
    { ssd1309_command(0x22); }
  else
    { ssd1309_command(0xF1); }
  ssd1309_command(SSD1309_SETVCOMDETECT);                 // 0xDB
  ssd1309_command(0x40);
  ssd1309_command(SSD1309_DISPLAYALLON_RESUME);           // 0xA4
  ssd1309_command(SSD1309_NORMALDISPLAY);                 // 0xA6

  ssd1309_command(SSD1309_DEACTIVATE_SCROLL);

  ssd1309_command(SSD1309_DISPLAYON);//--turn on oled panel
}


void OLED_I2C_SSD1309::invertDisplay(uint8_t i) {
  if (i) {
    ssd1309_command(SSD1309_INVERTDISPLAY);
  } else {
    ssd1309_command(SSD1309_NORMALDISPLAY);
  }
}

void OLED_I2C_SSD1309::ssd1309_command(uint8_t c) {
    // I2C
    uint8_t control = 0x00;   // Co = 0, D/C = 0
    Wire.beginTransmission(_i2caddr);
    Wire.write(control);
    Wire.write(c);
    Wire.endTransmission();
//}
}

// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void OLED_I2C_SSD1309::startscrollright(uint8_t start, uint8_t stop){
  ssd1309_command(SSD1309_RIGHT_HORIZONTAL_SCROLL);
  ssd1309_command(0X00);
  ssd1309_command(start);
  ssd1309_command(0X00);
  ssd1309_command(stop);
  ssd1309_command(0X00); // for SSD1309
  ssd1309_command(0X00);
  ssd1309_command(0XFF);
  ssd1309_command(SSD1309_ACTIVATE_SCROLL);
}

// startscrollleft
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void OLED_I2C_SSD1309::startscrollleft(uint8_t start, uint8_t stop){
  ssd1309_command(SSD1309_LEFT_HORIZONTAL_SCROLL);
  ssd1309_command(0X00);
  ssd1309_command(start);
  ssd1309_command(0X00);
  ssd1309_command(stop);
  ssd1309_command(0X00); // for SSD1309
  ssd1309_command(0X00);
  ssd1309_command(0XFF);
  ssd1309_command(SSD1309_ACTIVATE_SCROLL);
}


// startscrollverticaldown
// Activate a vertical scroll down for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollverticaldown(0x00, 0x0F)
void OLED_I2C_SSD1309::startscrolldown(uint8_t start, uint8_t stop){
  ssd1309_command(SSD1309_SET_VERTICAL_SCROLL_AREA);
  ssd1309_command(0X00);
  ssd1309_command(SSD1309_LCDHEIGHT);
  ssd1309_command(SSD1309_VERTICAL_SCROLL);
  ssd1309_command(0X00);
  ssd1309_command(start);
  ssd1309_command(0X00);
  ssd1309_command(stop);
  ssd1309_command(0X3F);
  ssd1309_command(0X00); // for SSD1309
  ssd1309_command(0X7F); // for SSD1309
  ssd1309_command(SSD1309_ACTIVATE_SCROLL);
}

// startscrollverticalup
// Activate a vertical scroll up for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollverticalup(0x00, 0x0F)
void OLED_I2C_SSD1309::startscrollup(uint8_t start, uint8_t stop){
  ssd1309_command(SSD1309_SET_VERTICAL_SCROLL_AREA);
  ssd1309_command(0X00);
  ssd1309_command(SSD1309_LCDHEIGHT);
  ssd1309_command(SSD1309_VERTICAL_SCROLL);
  ssd1309_command(0X00);
  ssd1309_command(start);
  ssd1309_command(0X00);
  ssd1309_command(stop);
  ssd1309_command(0X01);
  ssd1309_command(0X00); // for SSD1309
  ssd1309_command(0X7F); // for SSD1309
  ssd1309_command(SSD1309_ACTIVATE_SCROLL);
}


// startscrolldiagright
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void OLED_I2C_SSD1309::startscrolldiagright(uint8_t start, uint8_t stop){
  ssd1309_command(SSD1309_SET_VERTICAL_SCROLL_AREA);
  ssd1309_command(0X00);
  ssd1309_command(SSD1309_LCDHEIGHT);
  ssd1309_command(SSD1309_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
  ssd1309_command(0X01);
  ssd1309_command(start);
  ssd1309_command(0X00);
  ssd1309_command(stop);
  ssd1309_command(0X01);
  ssd1309_command(0X00); // for SSD1309
  ssd1309_command(0X7F); // for SSD1309
  ssd1309_command(SSD1309_ACTIVATE_SCROLL);
}

// startscrolldiagleft
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void OLED_I2C_SSD1309::startscrolldiagleft(uint8_t start, uint8_t stop){
  ssd1309_command(SSD1309_SET_VERTICAL_SCROLL_AREA);
  ssd1309_command(0X00);
  ssd1309_command(SSD1309_LCDHEIGHT);
  ssd1309_command(SSD1309_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
  ssd1309_command(0X01);
  ssd1309_command(start);
  ssd1309_command(0X00);
  ssd1309_command(stop);
  ssd1309_command(0X01);
  ssd1309_command(0X00); // for SSD1309
  ssd1309_command(0X7F); // for SSD1309
  ssd1309_command(SSD1309_ACTIVATE_SCROLL);
}

void OLED_I2C_SSD1309::stopscroll(void){
  ssd1309_command(SSD1309_DEACTIVATE_SCROLL);
}

// Dim the display
// dim = true: display is dimmed
// dim = false: display is normal
void OLED_I2C_SSD1309::dim(boolean dim) {
  uint8_t contrast;

  if (dim) {
    contrast = 0; // Dimmed display
  } else {
    if (_vccstate == SSD1309_EXTERNALVCC) {
      contrast = 0x9F;
    } else {
      contrast = 0xCF;
    }
  }
  // the range of contrast to too small to be really useful
  // it is useful to dim the display
  ssd1309_command(SSD1309_SETCONTRAST);
  ssd1309_command(contrast);
}

void OLED_I2C_SSD1309::show(void) {
  ssd1309_command(SSD1309_COLUMNADDR);
  ssd1309_command(0);   // Column start address (0 = reset)
  ssd1309_command(SSD1309_LCDWIDTH-1); // Column end address (127 = reset)

  ssd1309_command(SSD1309_PAGEADDR);
  ssd1309_command(0); // Page start address (0 = reset)
  #if SSD1309_LCDHEIGHT == 64
    ssd1309_command(7); // Page end address
  #endif
  #if SSD1309_LCDHEIGHT == 32
    ssd1309_command(3); // Page end address
  #endif
  #if SSD1309_LCDHEIGHT == 16
    ssd1309_command(1); // Page end address
  #endif

    // save I2C bitrate
#ifdef TWBR
    uint8_t twbrbackup = TWBR;
    TWBR = 12; // upgrade to 400KHz!
#endif

    //Serial.println(TWBR, DEC);
    //Serial.println(TWSR & 0x3, DEC);

    // I2C
    for (uint16_t i=0; i<(SSD1309_LCDWIDTH*SSD1309_LCDHEIGHT/8); i++) {
      // send a bunch of data in one xmission
      Wire.beginTransmission(_i2caddr);
      WIRE_WRITE(0x40);
      for (uint8_t x=0; x<16; x++) {
        WIRE_WRITE(buffer[i]);
        i++;
      }
      i--;
      Wire.endTransmission();
    }
#ifdef TWBR
    TWBR = twbrbackup;
#endif
//  }
}

void OLED_I2C_SSD1309::display(void) {
	show();
}



// clear everything
void OLED_I2C_SSD1309::clear(void) {
  memset(buffer, 0, (SSD1309_LCDWIDTH*SSD1309_LCDHEIGHT/8));
}

// clear everything
void OLED_I2C_SSD1309::clearDisplay(void) {
  clear();
}

void OLED_I2C_SSD1309::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  boolean bSwap = false;
  switch(rotation) {
    case 0:
      // 0 degree rotation, do nothing
      break;
    case 1:
      // 90 degree rotation, swap x & y for rotation, then invert x
      bSwap = true;
      ssd1309_swap(x, y);
      x = WIDTH - x - 1;
      break;
    case 2:
      // 180 degree rotation, invert x and y - then shift y around for height.
      x = WIDTH - x - 1;
      y = HEIGHT - y - 1;
      x -= (w-1);
      break;
    case 3:
      // 270 degree rotation, swap x & y for rotation, then invert y  and adjust y for w (not to become h)
      bSwap = true;
      ssd1309_swap(x, y);
      y = HEIGHT - y - 1;
      y -= (w-1);
      break;
  }

  if(bSwap) {
    drawFastVLineInternal(x, y, w, color);
  } else {
    drawFastHLineInternal(x, y, w, color);
  }
}

void OLED_I2C_SSD1309::drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color) {
  // Do bounds/limit checks
  if(y < 0 || y >= HEIGHT) { return; }

  // make sure we don't try to draw below 0
  if(x < 0) {
    w += x;
    x = 0;
  }

  // make sure we don't go off the edge of the display
  if( (x + w) > WIDTH) {
    w = (WIDTH - x);
  }

  // if our width is now negative, punt
  if(w <= 0) { return; }

  // set up the pointer for  movement through the buffer
  uint8_t *pBuf = buffer;
  // adjust the buffer pointer for the current row
  pBuf += ((y/8) * SSD1309_LCDWIDTH);
  // and offset x columns in
  pBuf += x;

  uint8_t mask = 1 << (y&7);

  switch (color)
  {
  case WHITE:         while(w--) { *pBuf++ |= mask; }; break;
    case BLACK: mask = ~mask;   while(w--) { *pBuf++ &= mask; }; break;
  case INVERSE:         while(w--) { *pBuf++ ^= mask; }; break;
  }
}

void OLED_I2C_SSD1309::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  bool bSwap = false;
  switch(rotation) {
    case 0:
      break;
    case 1:
      // 90 degree rotation, swap x & y for rotation, then invert x and adjust x for h (now to become w)
      bSwap = true;
      ssd1309_swap(x, y);
      x = WIDTH - x - 1;
      x -= (h-1);
      break;
    case 2:
      // 180 degree rotation, invert x and y - then shift y around for height.
      x = WIDTH - x - 1;
      y = HEIGHT - y - 1;
      y -= (h-1);
      break;
    case 3:
      // 270 degree rotation, swap x & y for rotation, then invert y
      bSwap = true;
      ssd1309_swap(x, y);
      y = HEIGHT - y - 1;
      break;
  }

  if(bSwap) {
    drawFastHLineInternal(x, y, h, color);
  } else {
    drawFastVLineInternal(x, y, h, color);
  }
}


void OLED_I2C_SSD1309::drawFastVLineInternal(int16_t x, int16_t __y, int16_t __h, uint16_t color) {

  // do nothing if we're off the left or right side of the screen
  if(x < 0 || x >= WIDTH) { return; }

  // make sure we don't try to draw below 0
  if(__y < 0) {
    // __y is negative, this will subtract enough from __h to account for __y being 0
    __h += __y;
    __y = 0;

  }

  // make sure we don't go past the height of the display
  if( (__y + __h) > HEIGHT) {
    __h = (HEIGHT - __y);
  }

  // if our height is now negative, punt
  if(__h <= 0) {
    return;
  }

  // this display doesn't need ints for coordinates, use local byte registers for faster juggling
  uint8_t y = __y;
  uint8_t h = __h;


  // set up the pointer for fast movement through the buffer
  uint8_t *pBuf = buffer;
  // adjust the buffer pointer for the current row
  pBuf += ((y/8) * SSD1309_LCDWIDTH);
  // and offset x columns in
  pBuf += x;

  // do the first partial byte, if necessary - this requires some masking
  uint8_t mod = (y&7);
  if(mod) {
    // mask off the high n bits we want to set
    mod = 8-mod;

    // note - lookup table results in a nearly 10% performance improvement in fill* functions
    // uint8_t mask = ~(0xFF >> (mod));
    static uint8_t premask[8] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
    uint8_t mask = premask[mod];

    // adjust the mask if we're not going to reach the end of this byte
    if( h < mod) {
      mask &= (0XFF >> (mod-h));
    }

  switch (color)
    {
    case WHITE:   *pBuf |=  mask;  break;
    case BLACK:   *pBuf &= ~mask;  break;
    case INVERSE: *pBuf ^=  mask;  break;
    }

    // fast exit if we're done here!
    if(h<mod) { return; }

    h -= mod;

    pBuf += SSD1309_LCDWIDTH;
  }


  // write solid bytes while we can - effectively doing 8 rows at a time
  if(h >= 8) {
    if (color == INVERSE)  {          // separate copy of the code so we don't impact performance of the black/white write version with an extra comparison per loop
      do  {
      *pBuf=~(*pBuf);

        // adjust the buffer forward 8 rows worth of data
        pBuf += SSD1309_LCDWIDTH;

        // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
        h -= 8;
      } while(h >= 8);
      }
    else {
      // store a local value to work with
      uint8_t val = (color == WHITE) ? 255 : 0;

      do  {
        // write our value in
      *pBuf = val;

        // adjust the buffer forward 8 rows worth of data
        pBuf += SSD1309_LCDWIDTH;

        // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
        h -= 8;
      } while(h >= 8);
      }
    }

  // now do the final partial byte, if necessary
  if(h) {
    mod = h & 7;
    // this time we want to mask the low bits of the byte, vs the high bits we did above
    // uint8_t mask = (1 << mod) - 1;
    // note - lookup table results in a nearly 10% performance improvement in fill* functions
    static uint8_t postmask[8] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
    uint8_t mask = postmask[mod];
    switch (color)
    {
      case WHITE:   *pBuf |=  mask;  break;
      case BLACK:   *pBuf &= ~mask;  break;
      case INVERSE: *pBuf ^=  mask;  break;
    }
  }
}

// ----- function Update -------------------------------------------------------
// draw a character
void OLED_I2C_SSD1309::__glcdCharX(uint8_t x, uint8_t y,uint8_t textSize, char c) {
  x=x*6;
  y=y<<3;
  for (uint8_t i =0; i<5; i++ ) {
    uint8_t line = pgm_read_byte(font+(c*5)+i);
    for (uint8_t j = 0; j<8; j++) {
      if (line & 0x1) 
      {
	     if (textSize == 1) // default size
		 {
	        drawPixel(x+i, y+j, _textColor);
		 }
	     else 
         {  // big size
	        fillRect(x+(i*textSize), y+(j*textSize), textSize, textSize, _textColor);
	     } 
      }
      else  // Addition test
      {
         if (textSize == 1) // default size
		{
	      drawPixel(x+i, y+j, _textBackgroundColor);
			}
	     else 
         {  // big size
	        fillRect(x+(i*textSize), y+(j*textSize), textSize, textSize, _textBackgroundColor);
	     } 
      }
      line >>= 1;
    }
  }
   for (uint8_t j = 0; j<8; j++)      // draw space vertical line 
   {
         if (textSize == 1) // default size
		 {
	        drawPixel(x+5, y+j, _textBackgroundColor);
		 }
	     else 
         {  // big size
	        fillRect(x+(5*textSize), y+(j*textSize), textSize, textSize, _textBackgroundColor);
	     } 

   }
}
// draw a character
void OLED_I2C_SSD1309::drawChar(uint8_t x, uint8_t y, char c) {
  for (uint8_t i =0; i<5; i++ ) {
    uint8_t line = pgm_read_byte(font+(c*5)+i);
    for (uint8_t j = 0; j<8; j++) {
      if (line & 0x1) 
      {
	     if (_textSize == 1) // default size
		 {
	        drawPixel(x+i, y+j, _textColor);
		 }
	     else 
         {  // big size
	        fillRect(x+(i*_textSize), y+(j*_textSize), _textSize, _textSize, _textColor);
	     } 
      }
      else  // Addition test
      {
         if (_textSize == 1) // default size
		{
	      drawPixel(x+i, y+j, _textBackgroundColor);
			}
	     else 
         {  // big size
	        fillRect(x+(i*_textSize), y+(j*_textSize), _textSize, _textSize, _textBackgroundColor);
	     } 
      }
      line >>= 1;
    }
  }
   for (uint8_t j = 0; j<8; j++)      // draw space vertical line 
   {
         if (_textSize == 1) // default size
		 {
	        drawPixel(x+5, y+j, _textBackgroundColor);
		 }
	     else 
         {  // big size
	        fillRect(x+(5*_textSize), y+(j*_textSize), _textSize, _textSize, _textBackgroundColor);
	     } 

   }
}

// draw String
/*
void OLED_I2C_SSD1309::drawString(uint8_t x, uint8_t y, char *c) {
	x = x*_textSize;
	y = y*_textSize+1;
  while (c[0] != 0) 
  {
    drawChar(x, y, c[0]);
    x += _textSize*6;
    c++;
    if ((x + (_textSize*5)) >= _width) 
	{
//      y += 10;
      y += _textSize*10;
      x = 0;
    }
  }
}
*/
// draw String
void OLED_I2C_SSD1309::drawString(uint8_t x, uint8_t y, String st) {
	x = x*_textSize;
	y = y*_textSize+1;
	const char * c = st.c_str();
  while (c[0] != 0) 
  {
    drawChar(x, y, c[0]);
    x += _textSize*6;
    c++;
    if ((x + (_textSize*5)) >= _width) 
	{
//      y += 10;
      y += _textSize*10;
      x = 0;
    }
  }
}
// Override with new method function call
void OLED_I2C_SSD1309::text(uint8_t _x, uint8_t _y ,const char *p,...)
{
	int char_count=0,x,y;
    char *arg,**pp;	// Poter of point
	char *ptr,buff[33];
	int argCnt=0;
	//glcdConfig();
	pp = (char **)(&p);
	ptr = (char *) p;	// Copy address
	arg = (char *)pp;	// Copy address of p point	

	va_list ap;
	va_start(ap, p); 
	
	/*if(_lcd_clear_display)
	{
		//_lcd_clear_display = 0;
		//lcd_clear();
	}*/
	arg += 2;	// Cross 2 time go to Origin of first parameter 

	_x = _x*_textSize;
	_y = _y*_textSize;

	while(*ptr)                 // Check data pointer = 0?
	{
	 	if(TEST_CHAR_TYPE(ptr))
		{
			x = (_x*(CHAR_PER_ROW*_textSize) + _y + (char_count*_textSize))%(CHAR_PER_ROW*_textSize);
			y = (_x*(CHAR_PER_ROW*_textSize) + _y + (char_count*_textSize))/(CHAR_PER_ROW*_textSize);
//			x = (_x*(CHAR_PER_ROW*_textSize))%(CHAR_PER_ROW*_textSize);
//			y = (_y + (char_count*_textSize))/(CHAR_PER_ROW*_textSize);
			char_count++;
       
       	    __glcdCharX(x,y,_textSize,toascii(*arg));
			arg+=2;	// Cross address char type
			ptr++;	// Cross %d parameter
		}
		else if(TEST_BIN_TYPE(ptr))
		{
			arg += 2;	// Cross 2 time go to Origin of first parameter 
			unsigned long ___ularg1;
			___ularg1= va_arg(ap, unsigned long);
			p = ultoa(___ularg1,&buff[0],2);
//             p = ultoa(*(unsigned int *)arg,&buff[0],2);
			while(*p)           // Check data pointer = 0?
			{
				x = (_x*(CHAR_PER_ROW*_textSize) + _y + (char_count*_textSize))%(CHAR_PER_ROW*_textSize);
				y = (_x*(CHAR_PER_ROW*_textSize) + _y + (char_count*_textSize))/(CHAR_PER_ROW*_textSize);
                char_count++;
			
            	__glcdCharX(x,y,_textSize,*p);
	 			p++;                     // Increase address 1 time 
			}
			arg+=2;	// Cross address int type
			ptr++;	// Cross %d parameter
			argCnt++; // increase arg counter
		}
		else if(TEST_HEX_TYPE(ptr))
		{
			arg += 2;	// Cross 2 time go to Origin of first parameter 
			unsigned long ___ularg1;
			___ularg1= va_arg(ap, unsigned long);
			p = ultoa(___ularg1,&buff[0],16);
//             p = ultoa(*(unsigned int *)arg,&buff[0],16);
			while(*p)           // Check data pointer = 0?
			{
				x = (_x*(CHAR_PER_ROW*_textSize) + _y + (char_count*_textSize))%(CHAR_PER_ROW*_textSize);
				y = (_x*(CHAR_PER_ROW*_textSize) + _y + (char_count*_textSize))/(CHAR_PER_ROW*_textSize);
                char_count++;
			
            	__glcdCharX(x,y,_textSize,*p);
	 			p++;                     // Increase address 1 time 
			}
			arg+=2;	// Cross address int type
			ptr++;	// Cross %d parameter
			argCnt++; // increase arg counter
		}
		else if(TEST_INT_TYPE(ptr))
		{
			arg += 2;	// Cross 2 time go to Origin of first parameter 
			long ___larg1;
			___larg1= va_arg(ap, long);
			p = ltoa(___larg1,&buff[0],10);
//             p = ltoa(*(long *)arg,&buff[0],10);
			while(*p)           // Check data pointer = 0?
			{
				x = (_x*(CHAR_PER_ROW*_textSize) + _y + (char_count*_textSize))%(CHAR_PER_ROW*_textSize);
				y = (_x*(CHAR_PER_ROW*_textSize) + _y + (char_count*_textSize))/(CHAR_PER_ROW*_textSize);
                char_count++;
			
            	__glcdCharX(x,y,_textSize,*p);
	 			p++;                     // Increase address 1 time 
			}
			arg+=2;	// Cross address int type
			ptr++;	// Cross %d parameter
			argCnt++; // increase arg counter
		}
		else if(TEST_LONG_TYPE(ptr))
		{
			arg += 2;	// Cross 2 time go to Origin of first parameter 
			long ___larg1;
			___larg1= va_arg(ap, long);
			p = ltoa(___larg1,&buff[0],10);
//			p = ltoa(*(long *)arg,&buff[0],10);
			while(*p)           // Check data pointer = 0?
			{
				x = (_x*(CHAR_PER_ROW*_textSize) + _y + (char_count*_textSize))%(CHAR_PER_ROW*_textSize);
				y = (_x*(CHAR_PER_ROW*_textSize) + _y + (char_count*_textSize))/(CHAR_PER_ROW*_textSize);
                char_count++;
			
            	__glcdCharX(x,y,_textSize,*p);
	 			p++;                     // Increase address 1 time 
			}
			arg+=2;	// Cross address long type
			ptr++;	// Cross %l parameter
			argCnt++; // increase arg counter
		}
		else if(TEST_FLOAT_TYPE(ptr))
		{
			arg+=2;	// Cross address long type
			double ___farg1;
			___farg1= va_arg(ap, double);
			p = dtostrf(___farg1,_floatPrecision+2,_floatPrecision,&buff[0]);	// Convert float to string(used libm.a)
//			p = dtostrf(*(double *)(arg),_floatPrecision+2,_floatPrecision,&buff[0]);	// Convert float to string(used libm.a)
			while(*p)           // Check data pointer = 0?
			{
				x = (_x*(CHAR_PER_ROW*_textSize) + _y + (char_count*_textSize))%(CHAR_PER_ROW*_textSize);
				y = (_x*(CHAR_PER_ROW*_textSize) + _y + (char_count*_textSize))/(CHAR_PER_ROW*_textSize);
                char_count++;
		
        		__glcdCharX(x,y,_textSize,*p);
	 			p++;                     // Increase address 1 time 
			}
			arg+=2;	// Cross address long type
			ptr++;	// Cross %l parameter
			argCnt++; // increase arg counter
		}
		/*else if(TEST_CLEAR_NEXT_TYPE(ptr))
		{
			_lcd_clear_display = 1;	// Clear display next time 
			
			ptr++;	// Cross %l parameter
		}
		else if(TEST_NEW_LINE_TYPE(ptr))
		{
			ptr++;	// Cross %l parameter
		}*/
		else
		{
				x = (_x*(CHAR_PER_ROW*_textSize) + _y + (char_count*_textSize))%(CHAR_PER_ROW*_textSize);
				y = (_x*(CHAR_PER_ROW*_textSize) + _y + (char_count*_textSize))/(CHAR_PER_ROW*_textSize);
			char_count++;
	
    		__glcdCharX(x,y,_textSize,*ptr);
	 	}
		ptr++;                      // Increase address 1 time 
	}
	va_end(ap);
}

char* OLED_I2C_SSD1309::___string2char(String command){
    if(command.length()!=0){
        char *p = const_cast<char*>(command.c_str());
        return p;
    }

    return NULL;
}
/*
void OLED_I2C_SSD1309::setTextColor(uint8_t newColor)
{
	_textColor = newColor;
}
void OLED_I2C_SSD1309::setTextColor(uint8_t newColor,uint8_t newBGColor)
{
	_textColor = newColor;
	_textBackgroundColor = newBGColor;
}
void OLED_I2C_SSD1309::setTextBackgroundColor(uint8_t newColor)
{
	_textBackgroundColor = newColor;
}
uint8_t OLED_I2C_SSD1309::getTextColor()
{
	return(_textColor);
}
uint8_t OLED_I2C_SSD1309::getTextBackgroundColor()
{
	return(_textBackgroundColor);
}
*/

void OLED_I2C_SSD1309::textColor(uint8_t newColor)
{
	_textColor = newColor;
	setTextColor(newColor);
}
void OLED_I2C_SSD1309::textColor(uint8_t newColor,uint8_t newBGColor)
{
	_textColor = newColor;
	_textBackgroundColor = newBGColor;
	setTextColor(newColor,newBGColor);
}
void OLED_I2C_SSD1309::textBackgroundColor(uint8_t newBGColor)
{
	_textBackgroundColor = newBGColor;
	setTextColor(_textColor,newBGColor);
}
void OLED_I2C_SSD1309::textSize(uint8_t newSize)
{
	_textSize = newSize;
	setTextSize(newSize);
}
void OLED_I2C_SSD1309::mode(uint8_t newMode)
{
    newMode = (newMode & 3);
    switch(newMode) {
        case 0:
        case 2:
			CHAR_PER_ROW=21;
            break;
        case 1:
        case 3:
			CHAR_PER_ROW=10;
            break;
    }	
	setRotation(newMode);
	_rotationMode=newMode;
}
uint8_t OLED_I2C_SSD1309::getmode(void)
{
	return _rotationMode;
}
