///////////////////////////////////////////////////////////////////////////////
//
// IOLib.h
//
// Authors: 	Roel Smeets
// Edit date: 	28-06-2025
//
///////////////////////////////////////////////////////////////////////////////

#ifndef IOLIB_H
#define IOLIB_H

#include <Arduino.h>

///////////////////////////////////////////////////////////////////////////////
// system #includes

#include <inttypes.h>

///////////////////////////////////////////////////////////////////////////////
// #define's

#define N_INPUT_BITS	6
#define N_OUTPUT_BITS	6

///////////////////////////////////////////////////////////////////////////////
// function prototypes

class dio{
public:
    void init(void);
    uint8_t getInput(void);
    bool isBitSet(uint8_t bitNumber);
    void setOutput(uint8_t value);
    void setBit(uint8_t bitNumber);
    void clearBit(uint8_t bitNumber);
    void toggleBit(uint8_t bitNumber);

    int16_t getGPIONumberInput(uint8_t inputBitNumber);
private:
    bool isValidBitNumber(uint8_t bitNumber);
    const uint8_t InputPins[N_INPUT_BITS] =
    {
        GPIO_NUM_36,	// LSB, bit 0
        GPIO_NUM_39,
        GPIO_NUM_34,
        GPIO_NUM_35,
        GPIO_NUM_32,
        GPIO_NUM_33,	// MSB, bit 5
    };

    const uint8_t OutputPins[N_OUTPUT_BITS] =
    {
        GPIO_NUM_25,	// LSB, bit 0
        GPIO_NUM_26,
        GPIO_NUM_27,
        GPIO_NUM_14,
        GPIO_NUM_12,
        GPIO_NUM_13,	// MSB, bit 5
    };
};
#endif	// IOLIB_H
