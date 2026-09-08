///////////////////////////////////////////////////////////////////////////////
//
// dio_lib.h
//
// Authors: 	Roel Smeets & Gerard Harkema
// Edit date: 	28-06-2025
// Revision: 	V2.0
// Modified: 	07-09-2026
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

class dio_device{
public:
    void Init(void);
    uint8_t GetInput(void);
    bool IsBitSet(uint8_t bitNumber);
    void SetOutput(uint8_t value);
    void SetBit(uint8_t bitNumber);
    void ClearBit(uint8_t bitNumber);
    void ToggleBit(uint8_t bitNumber);

    int16_t GetGPIONumberInput(uint8_t inputBitNumber);
private:
    bool IsValidBitNumber(uint8_t bitNumber);
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
