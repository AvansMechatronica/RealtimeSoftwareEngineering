/*
 * motor_control.cpp
 *
 * Created: 28-9-2023 15:37:48
 *  Authors: 	Roel Smeets & Gerard Harkema
 *  Revision: V2.0
 *  Modified: 07-09-2026
 */ 

///////////////////////////////////////////////////////////////////////////////
// system includes


#include <string.h>

///////////////////////////////////////////////////////////////////////////////
// application includes


#include "ts_printf.h"


///////////////////////////////////////////////////////////////////////////////
// application includes
#include "motor_control.h"

static HardwareConfig *hardwareConfig;

///////////////////////////////////////////////////////////////////////////////
// void MotorDisplayStatus(void)

void MotorInitialize(HardwareConfig *hardwareConfig)
{
    ::hardwareConfig = hardwareConfig;
}

void MotorDisplayStatus(void)
{
	uint8_t portInValue = 0;
	uint8_t bitVal		= 0;
	bool isSet			= false;
	
	// non-inverting input port, pull-up resistors
	
	portInValue = hardwareConfig->dio.GetInput();
	
	//led_DisplayValue(portInValue >> 1);	// using bits 1..4

	ts_printf("digital input = 0x%02x\n", portInValue);
	
	isSet = hardwareConfig->dio.IsBitSet(BIT_LIMIT_LEFT);
	bitVal = isSet? 1 : 0;
	ts_printf("Limit Left:     %d\n", bitVal);

	isSet = hardwareConfig->dio.IsBitSet(BIT_LIMIT_RIGHT);
	bitVal = isSet? 1 : 0;
	ts_printf("Limit Right:    %d\n", bitVal);

	isSet = hardwareConfig->dio.IsBitSet(BIT_ATOM_ERROR);
	bitVal = isSet? 1 : 0;
	ts_printf("Atom Error:     %d\n", bitVal);

	isSet = hardwareConfig->dio.IsBitSet(BIT_ESCON_OVERLOAD);
	bitVal = isSet? 1 : 0;
	ts_printf("ESCON Overload: %d\n", bitVal);
	
	ts_printf("\n");
}


///////////////////////////////////////////////////////////////////////////////
// bool MotorHasOverload(void)

bool MotorHasOverload(void)
{
	bool overload = true;

	overload = hardwareConfig->dio.IsBitSet(BIT_ESCON_OVERLOAD);
	
	return overload;
}


///////////////////////////////////////////////////////////////////////////////
// bool MotorIsAtLimit(motor_direction_t direction)

bool MotorIsAtLimit(motor_direction_t direction)
{
	bool atLimit = true;	// safe: assume at limit
	
	if (direction == MOVE_LEFT)
	{
		atLimit = hardwareConfig->dio.IsBitSet(BIT_LIMIT_LEFT);
	}
	else if (direction == MOVE_RIGHT)
	{
		atLimit = hardwareConfig->dio.IsBitSet(BIT_LIMIT_RIGHT);
	}
	
	if (atLimit)
	{
		//led_DisplayValue(0x0F);
	}
	
	return atLimit;
}


///////////////////////////////////////////////////////////////////////////////
// bool MotorMove(motor_direction_t direction)
//
// returns false if motor already at limit: movement NOT allowed
// returns true if motor not at limit: movement IS allowed

bool MotorMove(motor_direction_t direction)
{
	bool alreadyAtLimit = true;
	uint8_t dacChannel  = 0;
	float dacOutputVoltageLeft  =  -4.0;
	float dacOutputVoltageRight =   6.0;
	
	alreadyAtLimit = MotorIsAtLimit(direction);
	
	// only move motor if NOT at limit:
	if (alreadyAtLimit == false)
	{
		if (direction == MOVE_LEFT)
		{
			//led_DisplayValue(0x08);	// left LED on
			hardwareConfig->dac.SetOutputVoltage(dacChannel, dacOutputVoltageLeft);
		}
		else if (direction == MOVE_RIGHT)
		{
			//led_DisplayValue(0x01);	// right LED on
			hardwareConfig->dac.SetOutputVoltage(dacChannel, dacOutputVoltageRight);
		}
	}
	else	// safe default action if already at limit: stop
	{
		//led_DisplayValue(0x00);
		MotorStop();
	}
	
	return alreadyAtLimit;
}

///////////////////////////////////////////////////////////////////////////////
// void MotorGotoHomePosition(motor_direction_t direction)
//
// go to home position, either left or right

void MotorGotoHomePosition(motor_direction_t direction)
{
	MotorMove(direction);
	while (MotorIsAtLimit(direction) == false)
	{
		// do nothing, just keep going...
	}
	MotorStop();
	vTaskDelay(1000);	// allow for mechanical debounce...
}


///////////////////////////////////////////////////////////////////////////////
// void MotorStop(void)
//
// stop motor, set DAC output channel 0 to 0 Volt

void MotorStop(void)
{
	uint8_t dacChannel  = 0;
	float	dacValue	= 0.0;
	
	hardwareConfig->dac.SetOutputVoltage(dacChannel, dacValue);
	
	//led_DisplayValue(0x00);
}


///////////////////////////////////////////////////////////////////////////////
// void MotorEnableESCONController(void)
//
//enable ESCON controller via output port bit 0

void MotorEnableESCONController(void)
{
	hardwareConfig->dio.SetBit(BIT_ESCON_ENABLE);
}

///////////////////////////////////////////////////////////////////////////////
// void MotorDisableESCONController(void)
//
// disable ESCON controller via output port bit 0

void MotorDisableESCONController(void)
{
	hardwareConfig->dio.ClearBit(BIT_ESCON_ENABLE);
}
