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
#include "../config.h"
#include "fmap.h"
#include "../bits.h"

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

    uint16_t readRaw(uint8_t channel, uint8_t averageCount = 1);
    double readVoltage(uint8_t channel, uint8_t averageCount = 1);
    bool   isButtonPressed(uint8_t analogButton);