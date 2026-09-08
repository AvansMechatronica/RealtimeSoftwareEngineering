///////////////////////////////////////////////////////////////////////////////
//
// button_lib.cpp
//
// Authors: 	Roel Smeets & Gerard Harkema (Avans)
// Edit date: 	25-06-2025
// Revision: 	V2.0
// Modified: 	07-09-2026
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include <Arduino.h>

///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "button_lib.h"


///////////////////////////////////////////////////////////////////////////////
// void button::Init(adc3208 *adc)

button::button()
{
    // Constructor can be used to initialize any member variables if needed
}

void button::Init(adc3208 *adc)
{
    this->m_adc = adc;
	pinMode(BUTTON_PIN, INPUT_PULLUP); 
}


///////////////////////////////////////////////////////////////////////////////
// bool button::IsPressed(uint8_t buttonNumber)

bool button::IsPressed(uint8_t buttonNumber)
{
	bool isPressed = false;

	if (buttonNumber == 0)
	{
		isPressed = (digitalRead(BUTTON_PIN) == LOW);
	}
	else if ((buttonNumber == 1) || (buttonNumber == 2))
	{
		isPressed = m_adc->IsButtonPressed(buttonNumber);
	}
	
	return isPressed;
}
