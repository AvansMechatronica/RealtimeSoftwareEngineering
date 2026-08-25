///////////////////////////////////////////////////////////////////////////////
//
// SPILib.h
//
// Authors: 	Roel Smeets
// Edit date: 	25-06-2025
//
///////////////////////////////////////////////////////////////////////////////

#ifndef SPILIB_H_
#define SPILIB_H_

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include <Arduino.h>
#include <SPI.h>

///////////////////////////////////////////////////////////////////////////////
// #defines

#define SPI_MAX_DEVICENUMBER	7 	// max. 8 devices, do not use device 7 (!!)
#define SPI_N_SELECTBITS		3 	// means 3 bits required for selection

// select bits for 74HC138 MUX, select 1 of 8

#define SPI_SEL_2	GPIO_NUM_5
#define SPI_SEL_1	GPIO_NUM_17
#define SPI_SEL_0	GPIO_NUM_16

// device selection bits output of MUX

#define SPI_DEVICE_DAC01    0   // DAC channel 0 and 1
#define SPI_DEVICE_DAC23    1   // DAC channel 2 and 3
#define SPI_DEVICE_QC0      2   // Quadrature Counter 0
#define SPI_DEVICE_QC1      3   // Quadrature Counter 1
#define SPI_DEVICE_ADC      4   // 8 channel ADC
#define SPI_DEVICE_EXT_5    5   // external output for user device
#define SPI_DEVICE_EXT_6    6   // external output for user device
#define SPI_DEVICE_UNUSED   7   // used for deselecting all CS* signals!

#define SPI_DEVICE_CAN		SPI_DEVICE_EXT_5

// SPI pin definitions

#define VSPI_MISO	GPIO_NUM_19
#define VSPI_MOSI	GPIO_NUM_23
#define VSPI_SCLK	GPIO_NUM_18
#define VSPI_SS 	GPIO_NUM_5		// not used on board, done by MUX!

// SPI clock speed

#define SPI_DEFAULT_SPEED   4000000

///////////////////////////////////////////////////////////////////////////////
// function prototypes

class spi{
public:

    void init(void);
    void beginTransaction(SPISettings settings);
    void endTransaction(void);

    void writeByte(const uint8_t data);
    void writeWord(const uint16_t data);
    void readByte(uint8_t *byteData);
    void readWord(uint16_t *wordData);

    uint8_t transferByte(uint8_t byteToSend);
    uint16_t transferWord(uint16_t wordToSend);

    void selectDevice(uint8_t spiDeviceNumber);
    void deselectDevice(void);
protected:
    bool g_IsSPIInitialised = false;
    SPIClass vspi = SPIClass(VSPI); 		    // Use VSPI bus
    SPISettings Settings = SPISettings(SPI_DEFAULT_SPEED, MSBFIRST, SPI_MODE0); // default values
    const uint8_t SelectPins[SPI_N_SELECTBITS] =
    {
        SPI_SEL_0,		// LSB, bit 0
        SPI_SEL_1,
        SPI_SEL_2,		// MSB, bit 2
    };

};

#endif	// SPILIB_H_
