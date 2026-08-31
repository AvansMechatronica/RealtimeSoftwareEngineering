//////////////////////////////////////////////////////////////////////////////
//
// DAC4922Lib.h
//
// Authors: 	Roel Smeets
// Edit date: 	21-07-2025
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DAC4922_H
#define DAC4922_H

#include <SPI.h>
#include "spi_lib.h"
//#include "../config.h"
#include "fmap.h"
#include "bits.h"

///////////////////////////////////////////////////////////////////////////////
// DAC constants for MCP4922 & output stage

#define N_DAC_BITS			12
#define N_DAC_CHANNELS		4

#define DAC_MIN_CHANNEL		0 
#define DAC_MAX_CHANNEL		(N_DAC_CHANNELS - 1)

#define DAC_MIN_VALUE 		0
#define DAC_MAX_VALUE 		((1 << N_DAC_BITS) - 1)

#define DAC_SPAN            20.0    // -10 .. + 10 volt
#define DAC_RESOLUTION	    (DAC_SPAN / (DAC_MAX_VALUE + 1))

#define DAC_MIN_VOLTAGE 	-10.0
#define DAC_MAX_VOLTAGE 	(DAC_MIN_VOLTAGE + ((DAC_MAX_VALUE) * (DAC_RESOLUTION)))

///////////////////////////////////////////////////////////////////////////////
// defines for DAC MCP4922

#define DAC_SELECT_B      	 BIT_15 // bit 15 == 1: select DAC B
#define DAC_SELECT_A     	 0      // bit 15 == 0: select DAC A
	
#define DAC_VREF_BUFFERED    BIT_14 // bit 14 == 1: buffered Vref input
#define DAC_VREF_UNBUFFERED  0      // bit 14 == 0: unbuffered Vref input

#define DAC_GAINSELECT_1     BIT_13 // bit 13 == 1: output gain = 1
#define DAC_GAINSELECT_2     0      // bit 13 == 0: output gain = 2 

#define DAC_POWER_ON         BIT_12 // bit 12 == 1: enable output
#define DAC_POWER_DOWN       0      // bit 12 == 0: disable output buffer, output is Hi-Z


///////////////////////////////////////////////////////////////////////////////
// SPI settings for DAC MCP4922

#define SPI_DAC_SPEED	10000000

///////////////////////////////////////////////////////////////////////////////
// function prototypes

class dac4922{
public:
    void init(spi *spi_bus);
    void write(uint8_t dacChannel, uint16_t dacValue);
    void setOutputVoltage(uint8_t dacChannel, float outputVoltage);
    void setOutputVoltageAll(float outputVoltage);
private:
    uint8_t getSPIDeviceNumber(uint8_t dacChannel);
    spi *spi_bus;
    SPISettings DACSPISettings = SPISettings(SPI_DAC_SPEED, MSBFIRST, SPI_MODE0);
};

#endif  // DAC4922_H
