///////////////////////////////////////////////////////////////////////////////
//
// led_lib.h
//
// Authors: 	Roel Smeets & Gerard Harkema
// Edit date: 	28-06-2025
// Revision: 	V2.0
// Modified: 	07-09-2026
//
///////////////////////////////////////////////////////////////////////////////

#ifndef LEDLIB_H
#define LEDLIB_H

#include <Arduino.h>


///////////////////////////////////////////////////////////////////////////////
// defines

#define N_LEDS		2 		

#ifndef LED_PIN_ESP32_BOARD
#define LED_PCB		GPIO_NUM_2     // IO pin number on ESP32
#else
#define LED_PCB		LED_PIN_ESP32_BOARD
#endif

// LED numbers to use in led::Set(...)

#define LED_BLUE		0 	// use this one for blue LED on
#define LED_IO15		1 	// use this one for red LED on PCB


///////////////////////////////////////////////////////////////////////////////
// function prototypes

class led
{
public:
    led();
    void Init(void);
    void Set(uint8_t ledNumber, bool ledOn);
private:
    bool IsValidNumber(uint8_t ledNumber);
};


#endif	// LEDLIB_H
