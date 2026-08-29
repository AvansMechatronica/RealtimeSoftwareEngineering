///////////////////////////////////////////////////////////////////////////////
//
// button_lib.h
//
// Authors: 	Roel Smeets (Avans)
// Edit date: 	28-06-2025
//
///////////////////////////////////////////////////////////////////////////////

#ifndef BUTTONLIB_H_
#define BUTTONLIB_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

void switch_Init(void);
bool switch_IsPressed(uint8_t buttonNumber);

#ifdef __cplusplus
}

///////////////////////////////////////////////////////////////////////////////
// #defines

#define BUTTON_PIN		GPIO_NUM_4
#define N_BUTTONS       3   // 1 x digital, 2 x analog via 2 ADC channels


///////////////////////////////////////////////////////////////////////////////
// function prototypes
class button {
public:
    button();
    void init();
    bool isPressed(uint8_t buttonNumber);
};

#endif


#endif /* BUTTONLIB_H_ */