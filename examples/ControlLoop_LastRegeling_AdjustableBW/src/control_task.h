/*
 * control_task.h
 *
 * Created: 23-11-2023 14:40:03
 *  Authors: 	Roel Smeets & Gerard Harkema
 *  Revision: V2.0
 *  Modified: 07-09-2026
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