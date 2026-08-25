//////////////////////////////////////////////////////////////////////////////
//
// DAC4922Lib.cpp
//
// Authors: 	Roel Smeets
// Edit date: 	21-07-2025
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

void dac4922::init(spi *spi_bus)
{
	this->spi_bus = spi_bus;
	// init the DAC chips the first time by writing any value - use zero volts
	float outputVoltage = 0.0;
	
	for (uint8_t channel = 0; channel < N_DAC_CHANNELS; channel++)
	{
		setOutputVoltage(channel, outputVoltage);
	}
}


///////////////////////////////////////////////////////////////////////////////
// void dac4922::SelectSPIDevice(uint8_t dacChannel)

void dac4922::selectSPIDevice(uint8_t dacChannel)
{
	if (dacChannel < N_DAC_CHANNELS)
	{
		if ((dacChannel == 0) || (dacChannel == 1))
		{
			spi_bus->selectDevice(SPI_DEVICE_DAC01);
		}
		else if ((dacChannel == 2) || (dacChannel == 3))
		{
			spi_bus->selectDevice(SPI_DEVICE_DAC23);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// void dac4922::write(uint8_t dacChannel, uint16_t dacValue)

void dac4922::write(uint8_t dacChannel, uint16_t dacValue)
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

		spi_bus->beginTransaction(DACSPISettings);
		dac4922::selectSPIDevice(dacChannel);

		spi_bus->writeWord(dacCommand);

		spi_bus->deselectDevice(); // DEselect DAC channel: cause CSDAC* to go high!!
		spi_bus->endTransaction();
	}
}

///////////////////////////////////////////////////////////////////////////////
// void dac4922::SetOutputVoltage(uint8_t dacChannel, float outputVoltage)
//
// Vout = -10 + 8*Vdac

void dac4922::setOutputVoltage(uint8_t dacChannel, float outputVoltage)
{
	float dacValue = 0.0;
	
	outputVoltage = constrain(outputVoltage, DAC_MIN_VOLTAGE, DAC_MAX_VOLTAGE);
	
	dacValue = fmap(outputVoltage,	DAC_MIN_VOLTAGE, DAC_MAX_VOLTAGE, 
	 								DAC_MIN_VALUE, DAC_MAX_VALUE);	

	// SerialPrintf("DAC value channel %d = %d\n", dacChannel, (uint16_t)(dacValue));

	write(dacChannel, (uint16_t)dacValue);
}

///////////////////////////////////////////////////////////////////////////////
// void dac4922::SetOutputVoltageAll(float outputVoltage)

void dac4922::setOutputVoltageAll(float outputVoltage)
{
	uint8_t channel = 0;

	for (channel = 0; channel <= DAC_MAX_CHANNEL; channel++)
	{
		setOutputVoltage(channel, outputVoltage);
	}
}
