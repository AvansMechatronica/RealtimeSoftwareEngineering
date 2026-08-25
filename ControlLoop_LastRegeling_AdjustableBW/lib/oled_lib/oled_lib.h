/*
 * oled.h
 *
 * Created: 14-3-2023 06:24:46
 *  Author: Roel Smeets
 *  Adapted for ESP32 RPC Server project by Gerard Harkema
 */ 


#ifndef OLEDLIB_H_
#define OLEDLIB_H_

#include "SSD1306Wire.h"
///////////////////////////////////////////////////////////////////////////////
// #defines

#define ALIGN_LEFT				0
#define ALIGN_RIGHT				1
#define ALIGN_CENTER			2

#define OLED_NLINES				4

#define OLED_XSIZE				128
#define OLED_YSIZE				64

#define OLED_LINEHEIGTH 		(OLED_YSIZE / OLED_NLINES)

///////////////////////////////////////////////////////////////////////////////
// function prototypes

class oledDisplay {
public:
  bool init(void);
  void clear(void);
  void writeLine(uint8_t line, const char *message, uint8_t align);
protected:
  SSD1306Wire *display;
};


#endif /* OLEDLIB_H_ */