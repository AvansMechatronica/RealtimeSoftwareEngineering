///////////////////////////////////////////////////////////////////////////////
//
// ButtonLib.cpp
//
// Authors: 	Roel Smeets (Avans)
// Edit date: 	25-06-2025
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include <Arduino.h>

///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "button_lib.h"


///////////////////////////////////////////////////////////////////////////////
// void button_Init(void)

button::button()
{
    // Constructor can be used to initialize any member variables if needed
}

void button::init(adc3208 *adc)
{
    this->adc = adc;
	pinMode(BUTTON_PIN, INPUT_PULLUP); 
}


///////////////////////////////////////////////////////////////////////////////
// bool button_IsPressed(uint8_t buttonNumber)

bool button::isPressed(uint8_t buttonNumber)
{
	bool isPressed = false;

	if (buttonNumber == 0)
	{
		isPressed = (digitalRead(BUTTON_PIN) == LOW);
	}
	else if ((buttonNumber == 1) || (buttonNumber == 2))
	{
		// TODO: Implement ADC button reading for buttonNumber 1 and 2
		isPressed = adc->isButtonPressed(buttonNumber);
	}
	
	return isPressed;
}
