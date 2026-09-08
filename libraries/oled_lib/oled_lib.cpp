///////////////////////////////////////////////////////////////////////////////
//
// oled_lib.cpp
//
// Author:	 	Roel Smeets
// Edit date: 	13-03-2023
//				25-06-2025
// Adapted for ESP32 RPC Server project by Gerard Harkema
// Revision: 	V2.0
// Modified: 	07-09-2026
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// system #includes

#include "Arduino.h"


///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "oled_lib.h"

#define I2C_ADDRESS_OLED		0x3C	    // I2C address of OLED display
#define I2C_SDA_PIN			21		    // GPIO pin for I2C SDA
#define I2C_SCL_PIN			22		    // GPIO pin for I2C S


///////////////////////////////////////////////////////////////////////////////
// the OLED display object (class static member)


///////////////////////////////////////////////////////////////////////////////
// bool oledDisplay::Init(void)

bool oledDisplay::Init(void)
{
	bool result = false;
	
	display = new SSD1306Wire(I2C_ADDRESS_OLED, I2C_SDA_PIN, I2C_SCL_PIN);

	result = display->init();

	display->setFont(ArialMT_Plain_16);
	display->setContrast(250);
	display->setBrightness(255);
	display->setTextAlignment(TEXT_ALIGN_LEFT);
	display->flipScreenVertically();

	return result;
}

///////////////////////////////////////////////////////////////////////////////
// void oledDisplay::Clear(void)

void oledDisplay::Clear(void)
{
	display->clear();
	display->display();
}

//////////////////////////////////////////////////////////////////////////////
// void oledDisplay::WriteLine(uint8_t line, const char *message, uint8_t align)

void oledDisplay::WriteLine(uint8_t line, const char *message, uint8_t align)
{
	uint8_t startCol = 0;
	
	switch (align)
	{
		case ALIGN_LEFT:
		startCol = 0;
		display->setTextAlignment(TEXT_ALIGN_LEFT);
		break;
		
		case ALIGN_CENTER:
		startCol = OLED_XSIZE / 2;
		display->setTextAlignment(TEXT_ALIGN_CENTER);
		break;
		
		case ALIGN_RIGHT:
		startCol = OLED_XSIZE;
		display->setTextAlignment(TEXT_ALIGN_RIGHT);
		break;
	}

	line = constrain(line, 0, OLED_NLINES - 1);

	display->drawString(startCol, line * OLED_LINEHEIGTH, message);
	display->display();
}
