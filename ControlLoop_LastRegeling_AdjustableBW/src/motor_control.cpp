/*
 * motorControl.c
 *
 * Created: 28-9-2023 15:37:48
 *  Author: rasmsmee
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
// void motor_DisplayStatus(void)

void motor_initialize(HardwareConfig *hardwareConfig)
{
    ::hardwareConfig = hardwareConfig;
}

void motor_DisplayStatus(void)
{
	uint8_t portInValue = 0;
	uint8_t bitVal		= 0;
	bool isSet			= false;
	
	// non-inverting input port, pull-up resistors
	
	portInValue = hardwareConfig->dio.getInput();
	
	//led_DisplayValue(portInValue >> 1);	// using bits 1..4

	ts_printf("digital input = 0x%02x\n", portInValue);
	
	isSet = hardwareConfig->dio.isBitSet(BIT_LIMIT_LEFT);
	bitVal = isSet? 1 : 0;
	ts_printf("Limit Left:     %d\n", bitVal);

	isSet = hardwareConfig->dio.isBitSet(BIT_LIMIT_RIGHT);
	bitVal = isSet? 1 : 0;
	ts_printf("Limit Right:    %d\n", bitVal);

	isSet = hardwareConfig->dio.isBitSet(BIT_ATOM_ERROR);
	bitVal = isSet? 1 : 0;
	ts_printf("Atom Error:     %d\n", bitVal);

	isSet = hardwareConfig->dio.isBitSet(BIT_ESCON_OVERLOAD);
	bitVal = isSet? 1 : 0;
	ts_printf("ESCON Overload: %d\n", bitVal);
	
	ts_printf("\n");
}


///////////////////////////////////////////////////////////////////////////////
// bool motor_HasOverload(void)

bool motor_HasOverload(void)
{
	bool overload = true;

	overload = hardwareConfig->dio.isBitSet(BIT_ESCON_OVERLOAD);
	
	return overload;
}


///////////////////////////////////////////////////////////////////////////////
// bool motor_IsAtLimit(motor_direction_t direction)

bool motor_IsAtLimit(motor_direction_t direction)
{
	bool atLimit = true;	// safe: assume at limit
	
	if (direction == MOVE_LEFT)
	{
		atLimit = hardwareConfig->dio.isBitSet(BIT_LIMIT_LEFT);
	}
	else if (direction == MOVE_RIGHT)
	{
		atLimit = hardwareConfig->dio.isBitSet(BIT_LIMIT_RIGHT);
	}
	
	if (atLimit)
	{
		//led_DisplayValue(0x0F);
	}
	
	return atLimit;
}


///////////////////////////////////////////////////////////////////////////////
// bool motor_Move(motor_direction_t direction)
//
// returns false if motor already at limit: movement NOT allowed
// returns true if motor not at limit: movement IS allowed

bool motor_Move(motor_direction_t direction)
{
	bool alreadyAtLimit = true;
	uint8_t dacChannel  = 0;
	float dacOutputVoltageLeft  =  -4.0;
	float dacOutputVoltageRight =   6.0;
	
	alreadyAtLimit = motor_IsAtLimit(direction);
	
	// only move motor if NOT at limit:
	if (alreadyAtLimit == false)
	{
		if (direction == MOVE_LEFT)
		{
			//led_DisplayValue(0x08);	// left LED on
			hardwareConfig->dac.setOutputVoltage(dacChannel, dacOutputVoltageLeft);
		}
		else if (direction == MOVE_RIGHT)
		{
			//led_DisplayValue(0x01);	// right LED on
			hardwareConfig->dac.setOutputVoltage(dacChannel, dacOutputVoltageRight);
		}
	}
	else	// safe default action if already at limit: stop
	{
		//led_DisplayValue(0x00);
		motor_Stop();
	}
	
	return alreadyAtLimit;
}

///////////////////////////////////////////////////////////////////////////////
// void MotorGotoHomePosition(motor_direction_t direction)
//
// go to home position, either left or right

void motor_GotoHomePosition(motor_direction_t direction)
{
	motor_Move(direction);
	while (motor_IsAtLimit(direction) == false)
	{
		// do nothing, just keep going...
	}
	motor_Stop();
	vTaskDelay(1000);	// allow for mechanical debounce...
}


///////////////////////////////////////////////////////////////////////////////
// void motor_Stop(void)
//
// stop motor, set DAC output channel 0 to 0 Volt

void motor_Stop(void)
{
	uint8_t dacChannel  = 0;
	float	dacValue	= 0.0;
	
	hardwareConfig->dac.setOutputVoltage(dacChannel, dacValue);
	
	//led_DisplayValue(0x00);
}


///////////////////////////////////////////////////////////////////////////////
// void motor_EnableESCONController(void)
//
//enable ESCON controller via output port bit 0

void motor_EnableESCONController(void)
{
	hardwareConfig->dio.setBit(BIT_ESCON_ENABLE);
}

///////////////////////////////////////////////////////////////////////////////
// void motor_DisableESCONController(void)
//
// disable ESCON controller via output port bit 0

void motor_DisableESCONController(void)
{
	hardwareConfig->dio.clearBit(BIT_ESCON_ENABLE);
}
