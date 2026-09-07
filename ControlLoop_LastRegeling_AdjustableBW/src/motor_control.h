/*
 * motor_control.h
 *
 * Created: 28-9-2023 15:39:41
 *  Authors: 	Roel Smeets & Gerard Harkema
 *  Revision: V2.0
 *  Modified: 07-09-2026
 */ 


#ifndef MOTORCONTROL_H_
#define MOTORCONTROL_H_
#include "hardware_config.h"

///////////////////////////////////////////////////////////////////////////////
// #defines

#define BIT_LIMIT_LEFT		1	// bit positions in status input port
#define BIT_LIMIT_RIGHT		2
#define BIT_ATOM_ERROR		3
#define BIT_ESCON_OVERLOAD	4

#define BIT_ESCON_ENABLE	0	// bit positions in control output port
#define BIT_ESCON_POWERON	1

///////////////////////////////////////////////////////////////////////////////
// typedefs

typedef enum
{
	MOVE_LEFT,
	MOVE_RIGHT,
} motor_direction_t;


///////////////////////////////////////////////////////////////////////////////
// function prototypes

void MotorInitialize(HardwareConfig *hardwareConfig);
void MotorDisplayStatus(void);
bool MotorHasOverload(void);
bool MotorIsAtLimit(motor_direction_t direction);
bool MotorMove(motor_direction_t direction);
void MotorStop(void);
void MotorGotoHomePosition(motor_direction_t direction);
void MotorEnableESCONController(void);
void MotorDisableESCONController(void);

#endif /* MOTORCONTROL_H_ */
