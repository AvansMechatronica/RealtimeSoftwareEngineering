///////////////////////////////////////////////////////////////////////////////
//
// dio_lib.cpp
//
// Authors: 	Roel Smeets & Gerard Harkema
// Edit date: 	02-06-2025
// Revision: 	V2.0
// Modified: 	07-09-2026
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system #includes



///////////////////////////////////////////////////////////////////////////////
// application #includes

#include "dio_lib.h"



///////////////////////////////////////////////////////////////////////////////
// void dio_device::Init(void)

void dio_device::Init(void)
{
	uint8_t pin = 0;

	for (pin = 0; pin < N_INPUT_BITS; pin++)
	{
		//pinMode(InputPins[pin], INPUT_PULLDOWN); // Not available on ESP32
		pinMode(InputPins[pin], INPUT); 
	}

	for (pin = 0; pin < N_OUTPUT_BITS; pin++)
	{
		pinMode(OutputPins[pin], OUTPUT); 
	}
}

///////////////////////////////////////////////////////////////////////////////
// uint8_t dio_device::GetInput(void)

uint8_t dio_device::GetInput(void)
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
// bool dio_device::IsValidBitNumber(uint8_t bitNumber)

bool dio_device::IsValidBitNumber(uint8_t bitNumber)
{
	bool isValid = false;

	isValid = (bitNumber < N_INPUT_BITS);

	return isValid;
}

///////////////////////////////////////////////////////////////////////////////
// bool dio_device::IsBitSet(uint8_t bitNumber)

bool dio_device::IsBitSet(uint8_t bitNumber)
{
	bool isSet = false;

	if (IsValidBitNumber(bitNumber))
	{
		isSet = (digitalRead(bitNumber) == HIGH);
	}
	
	return isSet;
}

///////////////////////////////////////////////////////////////////////////////
// void dio_device::SetOutput(uint8_t value)

void dio_device::SetOutput(uint8_t value)
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
// void dio_device::SetBit(uint8_t bitNumber)

void dio_device::SetBit(uint8_t bitNumber)
{


	if (IsValidBitNumber(bitNumber))
	{
		digitalWrite(OutputPins[bitNumber], HIGH);
	}
}

///////////////////////////////////////////////////////////////////////////////
// void dio_device::ClearBit(uint8_t bitNumber)

void dio_device::ClearBit(uint8_t bitNumber)
{
	if (IsValidBitNumber(bitNumber))
	{
		digitalWrite(OutputPins[bitNumber], LOW);
	}
}

///////////////////////////////////////////////////////////////////////////////
// void dio_device::ToggleBit(uint8_t bitNumber)

void dio_device::ToggleBit(uint8_t bitNumber)
{
	if (IsValidBitNumber(bitNumber))
	{
		digitalWrite(OutputPins[bitNumber], !digitalRead(OutputPins[bitNumber]));
	}
}

///////////////////////////////////////////////////////////////////////////////
// int16_t dio_device::GetGPIONumberInput(uint8_t inputBitNumber)

int16_t dio_device::GetGPIONumberInput(uint8_t inputBitNumber)
{
	int16_t gpioNumber = -1;

	if (inputBitNumber < N_INPUT_BITS)
	{
		gpioNumber = InputPins[inputBitNumber];
	}

	return gpioNumber;
}
