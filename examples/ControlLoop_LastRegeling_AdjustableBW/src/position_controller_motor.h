/*
 * position_controller_motor.h
 *
 * Created: 18-9-2023 10:52:38
 *  Authors: 	Roel Smeets & Gerard Harkema
 *  Revision: V2.0
 *  Modified: 07-09-2026
 */ 


#ifndef POSITIONCONTROLLER_H_
#define POSITIONCONTROLLER_H_
#include "hardware_config.h"
///////////////////////////////////////////////////////////////////////////////
// function prototypes

void PosctrlInitialize(HardwareConfig *hardwareConfig);
void PosctrlInitParameters(double wbmFactor);
void PosctrlRunControllerMotorSide(void);

#endif /* POSITIONCONTROLLER_H_ */