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
All text above, and the splash screen must be included in any redistribution
*********************************************************************/
#ifndef _OLED_I2C_SSD1309_H_
#define _OLED_I2C_SSD1309_H_

#if ARDUINO >= 100
 #include "Arduino.h"
 #define WIRE_WRITE Wire.write
#else
 #include "WProgram.h"
  #define WIRE_WRITE Wire.send
#endif

#if defined(__SAM3X8E__)
 typedef volatile RwReg PortReg;
 typedef uint32_t PortMask;
 #define HAVE_PORTREG
#elif defined(ARDUINO_ARCH_SAMD)
// not supported
#elif defined(ESP8266) || defined(ESP32) || defined(ARDUINO_STM32_FEATHER) || defined(__arc__)
  typedef volatile uint32_t PortReg;
  typedef uint32_t PortMask;
#elif defined(__AVR__)
  typedef volatile uint8_t PortReg;
  typedef uint8_t PortMask;
  #define HAVE_PORTREG
#else
  // chances are its 32 bit so assume that
  typedef volatile uint32_t PortReg;
  typedef uint32_t PortMask;
#endif
#include "Arduino.h"
#include "Print.h"
#include <Adafruit_GFX.h>

#define BLACK 0
#define WHITE 1
#define INVERSE 2

#define SSD1309_I2C_ADDRESS   0x3C  // 011110+SA0+RW - 0x3C or 0x3D
// Address for 128x32 is 0x3C
// Address for 128x64 is 0x3D (default) or 0x3C (if SA0 is grounded)

/*=========================================================================
    SSD1309 Displays
    -----------------------------------------------------------------------
    The driver is used in multiple displays (128x64, 128x32, etc.).
    Select the appropriate display below to create an appropriately
    sized framebuffer, etc.

    SSD1309_128_64  128x64 pixel display

    SSD1309_128_32  128x32 pixel display

    SSD1309_96_16

    -----------------------------------------------------------------------*/
   #define SSD1309_128_64
//   #define SSD1309_128_32
//   #define SSD1309_96_16
/*=========================================================================*/

#if defined SSD1309_128_64 && defined SSD1309_128_32
  #error "Only one SSD1309 display can be specified at once in SSD1309.h"
#endif
#if !defined SSD1309_128_64 && !defined SSD1309_128_32 && !defined SSD1309_96_16
  #error "At least one SSD1309 display must be specified in SSD1309.h"
#endif

#if defined SSD1309_128_64
  #define SSD1309_LCDWIDTH                  128
  #define SSD1309_LCDHEIGHT                 64
#endif
#if defined SSD1309_128_32
  #define SSD1309_LCDWIDTH                  128
  #define SSD1309_LCDHEIGHT                 32
#endif
#if defined SSD1309_96_16
  #define SSD1309_LCDWIDTH                  96
  #define SSD1309_LCDHEIGHT                 16
#endif

#define SSD1309_SETCONTRAST 0x81
#define SSD1309_DISPLAYALLON_RESUME 0xA4
#define SSD1309_DISPLAYALLON 0xA5
#define SSD1309_NORMALDISPLAY 0xA6
#define SSD1309_INVERTDISPLAY 0xA7
#define SSD1309_DISPLAYOFF 0xAE
#define SSD1309_DISPLAYON 0xAF

#define SSD1309_SETDISPLAYOFFSET 0xD3
#define SSD1309_SETCOMPINS 0xDA

#define SSD1309_SETVCOMDETECT 0xDB

#define SSD1309_SETDISPLAYCLOCKDIV 0xD5
#define SSD1309_SETPRECHARGE 0xD9

#define SSD1309_SETMULTIPLEX 0xA8

#define SSD1309_SETLOWCOLUMN 0x00
#define SSD1309_SETHIGHCOLUMN 0x10

#define SSD1309_SETSTARTLINE 0x40

#define SSD1309_MEMORYMODE 0x20
#define SSD1309_COLUMNADDR 0x21
#define SSD1309_PAGEADDR   0x22

#define SSD1309_COMSCANINC 0xC0
#define SSD1309_COMSCANDEC 0xC8

#define SSD1309_SEGREMAP 0xA0

#define SSD1309_CHARGEPUMP 0x8D

#define SSD1309_EXTERNALVCC 0x1
#define SSD1309_SWITCHCAPVCC 0x2

// Scrolling #defines
#define SSD1309_ACTIVATE_SCROLL 0x2F
#define SSD1309_DEACTIVATE_SCROLL 0x2E
#define SSD1309_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1309_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1309_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1309_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1309_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A
#define SSD1309_VERTICAL_SCROLL 0x29


void setFloatPrecision(unsigned int prec);

class OLED_I2C_SSD1309 : public Adafruit_GFX {
 public:
//  OLED_I2C_SSD1309(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS);
//  OLED_I2C_SSD1309(int8_t DC, int8_t RST, int8_t CS);
  OLED_I2C_SSD1309(int8_t RST = -1);

  void begin() ;
  void begin(uint8_t switchvcc, uint8_t i2caddr, bool reset=true);
  void ssd1309_command(uint8_t c);

  void clear(void);
  void clearDisplay(void);
  void invertDisplay(uint8_t i);
  void show(void);
  void display(void);

  void startscrollright(uint8_t start, uint8_t stop);
  void startscrollleft(uint8_t start, uint8_t stop);
  void startscrolldown(uint8_t start, uint8_t stop);
  void startscrollup(uint8_t start, uint8_t stop);

  void startscrolldiagright(uint8_t start, uint8_t stop);
  void startscrolldiagleft(uint8_t start, uint8_t stop);
  void stopscroll(void);

  void dim(boolean dim);

  void drawPixel(int16_t x, int16_t y, uint16_t color);
  
  // ----- function Update -------------------------------------------------------
  void text(uint8_t _x, uint8_t _y ,const char *p,...);
  void drawChar(uint8_t x, uint8_t y, char c);
  void drawString(uint8_t x, uint8_t y, String st);
/*
  void setTextColor(uint8_t newColor);
  void setTextColor(uint8_t newColor,uint8_t newBGColor);
  void setTextBackgroundColor(uint8_t newColor);
  uint8_t getTextColor();
  uint8_t getTextBackgroundColor();
*/
  void textColor(uint8_t newColor);
  void textColor(uint8_t newColor,uint8_t newBGColor);
  void textBackgroundColor(uint8_t newColor);

  void textSize(uint8_t newSize);

  void mode(uint8_t newMode);
  uint8_t getmode(void);

  void __glcdCharX(uint8_t x, uint8_t y,uint8_t textSize, char c);
													  
  char* ___string2char(String command);
  //------------------------------------------------------------------------------ 		
  virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

 private:
//  int8_t _i2caddr, _vccstate, sid, sclk, dc, rst, cs;
  int8_t _i2caddr, _vccstate, rst;
//  void fastSPIwrite(uint8_t c);

//  boolean hwSPI;
#ifdef HAVE_PORTREG
//  PortReg *mosiport, *clkport, *csport, *dcport;
//  PortMask mosipinmask, clkpinmask, cspinmask, dcpinmask;
#endif

  inline void drawFastVLineInternal(int16_t x, int16_t y, int16_t h, uint16_t color) __attribute__((always_inline));
  inline void drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color) __attribute__((always_inline));

};

#endif /* _OLED_I2C_SSD1309_H_ */
 

