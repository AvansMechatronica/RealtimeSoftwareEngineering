///////////////////////////////////////////////////////////////////////////////
//
// button_lib.h
//
// Authors: 	Roel Smeets & Gerard Harkema (Avans)
// Edit date: 	28-06-2025
// Revision: 	V2.0
// Modified: 	09-09-2026
//
///////////////////////////////////////////////////////////////////////////////

#ifndef BUTTONLIB_H_
#define BUTTONLIB_H_

#include <stdint.h>

#include "adc_3208_lib.h"

///////////////////////////////////////////////////////////////////////////////
// #defines

#define BUTTON_PIN		GPIO_NUM_4
#define N_BUTTONS       3   // 1 x digital, 2 x analog via 2 ADC channels


///////////////////////////////////////////////////////////////////////////////
// function prototypes
class button {
public:
    button();
    void Init(adc3208 *adc);
    bool IsPressed(uint8_t buttonNumber);
private:
    adc3208 *m_adc;
};

#endif /* BUTTONLIB_H_ */