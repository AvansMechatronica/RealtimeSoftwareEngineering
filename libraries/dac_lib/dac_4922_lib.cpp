//////////////////////////////////////////////////////////////////////////////
//
// dac_4922_lib.cpp
//
// Authors: 	Roel Smeets & Gerard Harkema
// Edit date: 	21-07-2025
// Revision: 	V2.0
// Modified: 	07-09-2026
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include "Arduino.h"

///////////////////////////////////////////////////////////////////////////////
// application specific includes

#include "dac_4922_lib.h"

//#include "SerialPrintf.h"

///////////////////////////////////////////////////////////////////////////////
// SPI settings for DAC transactions



///////////////////////////////////////////////////////////////////////////////
// void dac4922::Init(void)

void dac4922::Init(spi_device *spi_bus)
{
	this->spi_bus = spi_bus;
	// Init the DAC chips the first time by writing any value - use zero volts
	float outputVoltage = 0.0;
	
	for (uint8_t channel = 0; channel < N_DAC_CHANNELS; channel++)
	{
		SetOutputVoltage(channel, outputVoltage);
	}
}


///////////////////////////////////////////////////////////////////////////////
// uint8_t dac4922::GetSPIDeviceNumber(uint8_t dacChannel)

uint8_t dac4922::GetSPIDeviceNumber(uint8_t dacChannel)
{
	if (dacChannel < N_DAC_CHANNELS)
	{
		if ((dacChannel == 0) || (dacChannel == 1))
		{
			return SPI_DEVICE_DAC01;
		}
		else if ((dacChannel == 2) || (dacChannel == 3))
		{
			return SPI_DEVICE_DAC23;
		}
	}
	return 0xff; // invalid device
}


///////////////////////////////////////////////////////////////////////////////
// void dac4922::Write(uint8_t dacChannel, uint16_t dacValue)

void dac4922::Write(uint8_t dacChannel, uint16_t dacValue)
{
	uint16_t dacCommand = 0;

	if (dacChannel < N_DAC_CHANNELS)
	{
		dacCommand  = dacValue & 0xfff; // only 12 bits allowed for DAC value
		dacCommand |= DAC_VREF_BUFFERED | DAC_GAINSELECT_1 | DAC_POWER_ON;

		if ((dacChannel == 1) || (dacChannel == 3) ) // channnel 1 or 3 => B channel of MCP4922
		{
			dacCommand = dacCommand | DAC_SELECT_B;
		}

		spi_bus->BeginTransaction(DACSPISettings, GetSPIDeviceNumber(dacChannel));
	

		spi_bus->WriteWord(dacCommand);

		//spi_bus->DeselectDevice(); // DEselect DAC channel: cause CSDAC* to go high!!
		spi_bus->EndTransaction();
	}
}

///////////////////////////////////////////////////////////////////////////////
// void dac4922::SetOutputVoltage(uint8_t dacChannel, float outputVoltage)
//
// Vout = -10 + 8*Vdac

void dac4922::SetOutputVoltage(uint8_t dacChannel, float outputVoltage)
{
	float dacValue = 0.0;
	
	outputVoltage = constrain(outputVoltage, DAC_MIN_VOLTAGE, DAC_MAX_VOLTAGE);
	
	dacValue = fmap(outputVoltage,	DAC_MIN_VOLTAGE, DAC_MAX_VOLTAGE, 
	 								DAC_MIN_VALUE, DAC_MAX_VALUE);	

	// SerialPrintf("DAC value channel %d = %d\n", dacChannel, (uint16_t)(dacValue));

	Write(dacChannel, (uint16_t)dacValue);
}

///////////////////////////////////////////////////////////////////////////////
// void dac4922::SetOutputVoltageAll(float outputVoltage)

void dac4922::SetOutputVoltageAll(float outputVoltage)
{
	uint8_t channel = 0;

	for (channel = 0; channel <= DAC_MAX_CHANNEL; channel++)
	{
		SetOutputVoltage(channel, outputVoltage);
	}
}
