/*
 * MotroControl.c
 *
 * Created: 28-9-2023 15:37:48
 *  Author: rasmsmee
 */ 

///////////////////////////////////////////////////////////////////////////////
// system includes

#include <asf.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////
// application includes

#include "CommandConsole.h"
#include "vPrintString.h"
#include "TaskSleep.h"

///////////////////////////////////////////////////////////////////////////////
// HAL includes for RTSW board

#include "DeviceIOLib.h"
#include "ADCLib.h"
#include "DAC4921Lib.h"
#include "SPILib.h"
#include "LEDLib.h"
#include "SwitchLib.h"
#include "PortIOLib.h"
#include "QC7366Lib.h"
#include "InterruptLib.h"
#include "I2CLib.h"
#include "GyroFXASLib.h"
#include "StatusLED.h"

#include "MotorControl.h"

///////////////////////////////////////////////////////////////////////////////
// void motor_DisplayStatus(void)

void motor_DisplayStatus(void)
{
	uint8_t portInValue = 0;
	uint8_t bitVal		= 0;
	bool isSet			= false;
	
	// non-inverting input port, pull-up resistors
	
	portInValue = port_GetInput();
	
	led_DisplayValue(portInValue >> 1);	// using bits 1..4

	vPrintString("digital input = 0x%02x\n", portInValue);
	
	isSet = port_IsBitSet(BIT_LIMIT_LEFT);
	bitVal = isSet? 1 : 0;
	vPrintString("Limit Left:     %d\n", bitVal);

	isSet = port_IsBitSet(BIT_LIMIT_RIGHT);
	bitVal = isSet? 1 : 0;
	vPrintString("Limit Right:    %d\n", bitVal);

	isSet = port_IsBitSet(BIT_ATOM_ERROR);
	bitVal = isSet? 1 : 0;
	vPrintString("Atom Error:     %d\n", bitVal);

	isSet = port_IsBitSet(BIT_ESCON_OVERLOAD);
	bitVal = isSet? 1 : 0;
	vPrintString("ESCON Overload: %d\n", bitVal);
	
	vPrintString("\n");
}


///////////////////////////////////////////////////////////////////////////////
// bool motor_HasOverload(void)

bool motor_HasOverload(void)
{
	bool overload = true;

	overload = port_IsBitSet(BIT_ESCON_OVERLOAD);
	
	return overload;
}


///////////////////////////////////////////////////////////////////////////////
// bool motor_IsAtLimit(motor_direction_t direction)

bool motor_IsAtLimit(motor_direction_t direction)
{
	bool atLimit = true;	// safe: assume at limit
	
	if (direction == MOVE_LEFT)
	{
		atLimit = port_IsBitSet(BIT_LIMIT_LEFT);
	}
	else if (direction == MOVE_RIGHT)
	{
		atLimit = port_IsBitSet(BIT_LIMIT_RIGHT);
	}
	
	if (atLimit)
	{
		led_DisplayValue(0x0F);
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
	float dacOutputVoltage_1 =  -4.0;
	float dacOutputVoltage_2 =   6.0;
	
	alreadyAtLimit = motor_IsAtLimit(direction);
	
	// only move motor if NOT at limit:
	if (alreadyAtLimit == false)
	{
		// TODO: start motor in correct direction with DAC...
		if (direction == MOVE_LEFT)
		{
			led_DisplayValue(0x08);
			for (dacChannel = 0; dacChannel <= DAC_MAX_CHANNEL; dacChannel++)
			{
				dac_SetOutputVoltage(dacChannel, dacOutputVoltage_1);
			}
		}
		else if (direction == MOVE_RIGHT)
		{
			led_DisplayValue(0x01);
			for (dacChannel = 0; dacChannel <= DAC_MAX_CHANNEL; dacChannel++)
			{
				dac_SetOutputVoltage(dacChannel, dacOutputVoltage_2);
			}
		}
	}
	else	// safe default action if already at limit: stop
	{
		motor_Stop();
	}
	
	return alreadyAtLimit;
}


///////////////////////////////////////////////////////////////////////////////
// void motor_Stop(void)
//
// stop motor, set DAC output channel 0 to 0 Volt

void motor_Stop(void)
{
	uint8_t dacChannel  = 0;
	float	dacValue	= 0.0;
	
	for (dacChannel = 0; dacChannel <= DAC_MAX_CHANNEL; dacChannel++)
	{
		dac_SetOutputVoltage(dacChannel, dacValue);
	}
	
	led_DisplayValue(0x00);
}


///////////////////////////////////////////////////////////////////////////////
// void motor_EnableESCONController(void)
//
//enable ESCON controller via output port bit 0

void motor_EnableESCONController(void)
{
	port_SetBit(BIT_ESCON_ENABLE, true);
}

///////////////////////////////////////////////////////////////////////////////
// void motor_DisableESCONController(void)
//
// disable ESCON controller via output port bit 0

void motor_DisableESCONController(void)
{
	port_SetBit(BIT_ESCON_ENABLE, false);
}
