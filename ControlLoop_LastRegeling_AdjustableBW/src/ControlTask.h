/*
 * ControlTask.h
 *
 * Created: 23-11-2023 14:40:03
 *  Author: rasmsmee
 */ 


#ifndef CONTROLTASK_H_
#define CONTROLTASK_H_

#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// function prototypes

void ClockInterruptHandler(void);
bool InitializePeriodicTimer(uint32_t intervalUs);
void ControlLoop(void);
void ControlTask(void *pvParameters);

#endif /* CONTROLTASK_H_ */