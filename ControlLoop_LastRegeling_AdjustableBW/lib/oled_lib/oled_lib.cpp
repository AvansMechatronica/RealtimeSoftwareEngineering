///////////////////////////////////////////////////////////////////////////////
//
// oled.cpp
//
// Author:	 	Roel Smeets
// Edit date: 	13-03-2023
//				25-06-2025
// Adapted for ESP32 RPC Server project by Gerard Harkema
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
// bool oled_Init(void)

bool oledDisplay::init(void)
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
// void oled_Clear(void)

void oledDisplay::clear(void)
{
	display->clear();
	display->display();
}

//////////////////////////////////////////////////////////////////////////////
// oled_writeLine(uint8_t row, const char *message, uint8_t align)

void oledDisplay::writeLine(uint8_t line, const char *message, uint8_t align)
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
