//////////////////////////////////////////////////////////////////////////////
//
// ADC3208Lib.h
//
// Authors: 	Roel Smeets
// Edit date: 	21-07-2025
//
///////////////////////////////////////////////////////////////////////////////

#ifndef ADC3208_H
#define ADC3208_H

#include <SPI.h>
#include "spi_lib.h"
#include "fmap.h"
#include "bits.h"


///////////////////////////////////////////////////////////////////////////////
// ADC constants & definitions for MCP3208 8 channel ADC

#define N_ADC_CHANNELS      8
#define N_ADC_BITS		    12

#define ADC_MIN_CHANNEL		0
#define ADC_MAX_CHANNEL		(N_ADC_CHANNELS - 1)

#define ADC_MIN_VALUE 	    0
#define ADC_MAX_VALUE       ((1 << N_ADC_BITS) - 1)

#define ADC_REFERENCE_VOLTAGE   2.5

// definitions for channels 0..3 with input range from -10 .. +10 volt

#define ADC03_MIN_VOLTAGE       -10.0
#define ADC03_RESOLUTION        ((8.0 * ADC_REFERENCE_VOLTAGE) / (ADC_MAX_VALUE + 1))
#define ADC03_MAX_VOLTAGE		(ADC03_MIN_VOLTAGE + ((ADC_MAX_VALUE) * (ADC03_RESOLUTION)))

// definitions for channels 4..7 with input range from 0 .. +2.5 volt

#define ADC47_RESOLUTION        (ADC_REFERENCE_VOLTAGE / (ADC_MAX_VALUE + 1))
#define ADC47_MIN_VOLTAGE	    0.0
#define ADC47_MAX_VOLTAGE		((ADC_MAX_VALUE) * (ADC47_RESOLUTION))

///////////////////////////////////////////////////////////////////////////////
// conversion factors

#define VOLT_TO_MV			(1e3)
#define MV_TO_VOLT			(1e-3)

///////////////////////////////////////////////////////////////////////////////
// bit defines for ADC MCP3208

#define ADC_STR     BIT_10
#define ADC_SINGLE  BIT_9

///////////////////////////////////////////////////////////////////////////////
// SPI settings for ADC MCP3208

#define SPI_ADC_SPEED	2000000

///////////////////////////////////////////////////////////////////////////////
// function prototypes

class adc3208{
public:

    void init(spi *spi_bus);

    uint16_t readRaw(uint8_t channel, uint8_t averageCount = 1);
    void readRawMultiple(uint8_t channelList[], uint8_t numChannels, uint16_t rawValues[]);
    void readVoltageMultiple(uint8_t channelList[], uint8_t numChannels, double voltages[]);

    double readVoltage(uint8_t channel, uint8_t averageCount = 1);
    bool   isButtonPressed(uint8_t analogButton);
private:
    spi *spi_bus;
    SPISettings ADCSPISettings = SPISettings(SPI_ADC_SPEED, MSBFIRST, SPI_MODE0);
    double rawToVoltage(uint16_t adcRaw, uint8_t channel);
};

#endif  // ADC3208_H

//   uint16_t readRaw(uint8_t channel, uint8_t averageCount = 1);
//    double readVoltage(uint8_t channel, uint8_t averageCount = 1);
//    bool   isButtonPressed(uint8_t analogButton);