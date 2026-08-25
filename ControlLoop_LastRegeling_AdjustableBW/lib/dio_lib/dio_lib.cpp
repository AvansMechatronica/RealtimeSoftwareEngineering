///////////////////////////////////////////////////////////////////////////////
//
// IOLib.cpp
//
// Authors: 	Roel Smeets
// Edit date: 	02-06-2025
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system #includes



///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "dio_lib.h"



///////////////////////////////////////////////////////////////////////////////
// void dio::Init(void)

void dio::init(void)
{
	uint8_t pin = 0;

	for (pin = 0; pin < N_INPUT_BITS; pin++)
	{
		pinMode(InputPins[pin], INPUT_PULLDOWN); 
	}

	for (pin = 0; pin < N_OUTPUT_BITS; pin++)
	{
		pinMode(OutputPins[pin], OUTPUT); 
	}
}

///////////////////////////////////////////////////////////////////////////////
// uint8_t dio::GetInput(void)

uint8_t dio::getInput(void)
{
	uint8_t value = 0;
	uint8_t bitNr = 0;

	for (bitNr = 0; bitNr < N_INPUT_BITS; bitNr++)
	{
		if (digitalRead(InputPins[bitNr]) == HIGH)
		{
			value = value | (0x01 << bitNr);
		}
	}

	return value;
}

///////////////////////////////////////////////////////////////////////////////
// bool dio::isValidBitNumber(uint8_t bitNumber)

bool dio::isValidBitNumber(uint8_t bitNumber)
{
	bool isValid = false;

	isValid = (bitNumber < N_INPUT_BITS);

	return isValid;
}

///////////////////////////////////////////////////////////////////////////////
// bool dio::IsBitSet(uint8_t bitNumber)

bool dio::isBitSet(uint8_t bitNumber)
{
	bool isBitSet = false;

	if (isValidBitNumber(bitNumber))
	{
		isBitSet = (digitalRead(bitNumber) == HIGH);
	}
	
	return isBitSet;
}

///////////////////////////////////////////////////////////////////////////////
// void dio::SetOutput(uint8_t value)

void dio::setOutput(uint8_t value)
{
	uint8_t bitNr = 0;
	uint8_t bitOn = LOW;

	for(bitNr = 0; bitNr < N_OUTPUT_BITS; bitNr++)
	{
		if ((value & (0x01 << bitNr)) != 0)
		{
			bitOn = HIGH;
		}
		else
		{
			bitOn = LOW;
		}
		digitalWrite(OutputPins[bitNr], bitOn);
	}
}

///////////////////////////////////////////////////////////////////////////////
// void dio::setBit(uint8_t bitNumber)

void dio::setBit(uint8_t bitNumber)
{


	if (isValidBitNumber(bitNumber))
	{
		digitalWrite(OutputPins[bitNumber], HIGH);
	}
}

///////////////////////////////////////////////////////////////////////////////
// void dio::clearBit(uint8_t bitNumber)

void dio::clearBit(uint8_t bitNumber)
{
	if (isValidBitNumber(bitNumber))
	{
		digitalWrite(OutputPins[bitNumber], LOW);
	}
}

///////////////////////////////////////////////////////////////////////////////
// void dio::toggleBit(uint8_t bitNumber)

void dio::toggleBit(uint8_t bitNumber)
{
	if (isValidBitNumber(bitNumber))
	{
		digitalWrite(OutputPins[bitNumber], !digitalRead(OutputPins[bitNumber]));
	}
}

///////////////////////////////////////////////////////////////////////////////
// int16_t dio::getGpioNumberInput(uint8_t inputBitNumber)

int16_t dio::getGPIONumberInput(uint8_t inputBitNumber)
{
	int16_t gpioNumber = -1;

	if (inputBitNumber < N_INPUT_BITS)
	{
		gpioNumber = InputPins[inputBitNumber];
	}

	return gpioNumber;
}
