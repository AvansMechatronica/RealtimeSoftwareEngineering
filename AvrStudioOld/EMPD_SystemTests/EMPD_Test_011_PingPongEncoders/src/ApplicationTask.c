/*
 * ApplicationTask.c
 *
 * Created: 10-9-2023 09:48:15
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


///////////////////////////////////////////////////////////////////////////////
// typedefs

typedef enum
{
	MOVE_LEFT,
	MOVE_RIGHT,
} motor_direction_t;


///////////////////////////////////////////////////////////////////////////////
// #defines

#define BIT_LIMIT_LEFT		1	// bit positions in status input port
#define BIT_LIMIT_RIGHT		2
#define BIT_ATOM_ERROR		3
#define BIT_ESCON_OVERLOAD	4


#define BIT_ESCON_ENABLE	0	// bit positions in control output port
#define BIT_ESCON_POWERON	1


///////////////////////////////////////////////////////////////////////////////
// function prototypes

void ApplicationTask(void *pvParameters);
void interruptHandler(uint32_t id, uint32_t mask);

void test_02_PingPongTest(void);

bool IsAtLimit(motor_direction_t direction);
bool MotorMove(motor_direction_t direction);
bool SafeMotorMove(motor_direction_t direction);
void MotorStop(void);
bool Overload(void);
void EnableESCONController(void);
void DisableESCONController(void);
void DisplayStatus(void);

///////////////////////////////////////////////////////////////////////////////
// void DisplayStatus(void)

void DisplayStatus(void)
{
	uint8_t bitVal = 0;
	bool isSet = false;
	uint8_t portInValue = 0;
		
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
// bool Overload(void)

bool Overload(void)
{
	bool overload = true;

	overload = port_IsBitSet(BIT_ESCON_OVERLOAD);
	
	return overload;
}

///////////////////////////////////////////////////////////////////////////////
// bool IsAtLimit(motor_direction_t direction)

bool IsAtLimit(motor_direction_t direction)
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
// bool MotorMove(motor_direction_t direction)
//
// returns false if motor already at limit: movement NOT allowed
// returns true if motor not at limit: movement IS allowed

bool MotorMove(motor_direction_t direction)
{
	bool alreadyAtLimit = true;
	uint8_t dacChannel = 0;
	float dacOutputVoltage_1 =  -2.5;
	float dacOutputVoltage_2 =   3.0;
	
	alreadyAtLimit = IsAtLimit(direction);
	
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
		MotorStop();
	}
	
	return alreadyAtLimit;
}

///////////////////////////////////////////////////////////////////////////////
// void MotorStop(void)
//
// stop motor, set DAC output channels 0 to 0 Volt

void MotorStop(void)
{
	uint8_t dacChannel = 0;
	float dacValue = 0.0;
	
	for (dacChannel = 0; dacChannel <= DAC_MAX_CHANNEL; dacChannel++)
	{
		dac_SetOutputVoltage(dacChannel, dacValue);
	}
	
	led_DisplayValue(0x00);
}


///////////////////////////////////////////////////////////////////////////////
// void MotorGotoHomePosition(motor_direction_t direction)
//
// go to home position, either left or right

void MotorGotoHomePosition(motor_direction_t direction)
{
	MotorMove(direction);
	while (IsAtLimit(direction) == false)
	{
		// do nothing, just keep going...
	}
	MotorStop();
	taskSleep(1000);	// allow for mechanical debounce...
}


///////////////////////////////////////////////////////////////////////////////
// void EnableESCONController(void)
//
//	enable ESCON controller via output port bit 0

void EnableESCONController(void)
{
	port_SetBit(BIT_ESCON_ENABLE, true);
}


///////////////////////////////////////////////////////////////////////////////
// void DisableESCONController(void)
//
// disable ESCON controller via output port bit 0	

void DisableESCONController(void)
{
	port_SetBit(BIT_ESCON_ENABLE, false);
}


///////////////////////////////////////////////////////////////////////////////
// void QCEncodersSetup(void)

void QCEncodersSetup(void)
{
	uint8_t qcChannel     = 0;
	uint8_t	qcDefaultMode = 0;
	mode_register_t qcModeRegister = QC_MODE_REGISTER_0;
		
	qcDefaultMode = MODE_QC_4 | MODE_FREERUNNING | INDEX_DISABLE | INDEX_ASYNC | FILTERCLOCK_DIV_2;

	for (qcChannel = 0; qcChannel <= QC_MAX_CHANNEL; qcChannel++)
	{
		qc_WriteModeRegister(qcChannel, qcModeRegister, qcDefaultMode);
		qc_EnableCounter(qcChannel);
		qc_ClearCountRegister(qcChannel);
	}	
}

///////////////////////////////////////////////////////////////////////////////
// void QCEncodersShowCount(const char *idString)

void QCEncodersShowCount(const char *idString)
{
	int32_t qcCountRegister = 0;
	uint8_t qcChannel = 0;

	for (qcChannel = 0; qcChannel <= QC_MAX_CHANNEL; qcChannel++)
	{
		qcCountRegister = qc_ReadCountRegister(qcChannel);
		vPrintString("%s >>> channel %d: CNT = %8d\n", idString, qcChannel, qcCountRegister);
	}
}

///////////////////////////////////////////////////////////////////////////////
// void QCEncodersClearCount(const char *idString)

void QCEncodersClearCount(void)
{
	uint8_t qcChannel = 0;

	for (qcChannel = 0; qcChannel <= QC_MAX_CHANNEL; qcChannel++)
	{
		qc_ClearCountRegister(qcChannel);
	}
}


///////////////////////////////////////////////////////////////////////////////
// void test_02_PingPongTestEncoders(void)
	
 void test_02_PingPongTestEncoders(void)
 {
	bool atLimit = true;

	DisableESCONController();

	QCEncodersSetup();

	EnableESCONController();
	
	MotorGotoHomePosition(MOVE_RIGHT);

	QCEncodersClearCount();
	QCEncodersShowCount("HOME counts >>>");
	
	while (true)
	{
		if (Overload())
		{
			vPrintString("*** ESCON overload ***\n");
		}
		 
		atLimit = MotorMove(MOVE_LEFT);
		if (atLimit)
		{
			vPrintString("> already at left limit\n");
		}
		else
		{
			vPrintString("> moving to the left\n");
		}
		 
		while (IsAtLimit(MOVE_LEFT) == false)
		{
			// do nothing, just keep going...
		}
		vPrintString("> left limit reached\n");

		MotorStop();
		vPrintString("> stopped\n");
		taskSleep(1000);
		
		QCEncodersShowCount("LEFT >>>");
		 
		atLimit = MotorMove(MOVE_RIGHT);
		if (atLimit)
		{
			vPrintString("> already at right limit\n");
		}
		else
		{
			vPrintString("> moving to the right\n");
		}
		 
		while (IsAtLimit(MOVE_RIGHT) == false)
		{
			// do nothing, just keep going...
		}
		vPrintString("> right limit reached\n");

		MotorStop();
		vPrintString("> stopped\n");
		taskSleep(1000); 

		QCEncodersShowCount("RIGHT >>>");
	 }
	 
 }
 
/*

uint32_t int_pin = 0;
uint32_t g_InterruptCount = 0;
uint32_t g_id  = 0;
uint32_t g_mask = 0;

void interruptHandler(uint32_t id, uint32_t mask)
{
	g_InterruptCount++;
	g_id = id;
	g_mask = mask;
}

void test_07_InterruptTest(void)
{
	uint32_t flags = PIO_IT_FALL_EDGE | PIO_DEBOUNCE;
	
	interrupt_AttachHandler(interruptHandler, PIN_A6, flags);
	interrupt_AttachHandler(interruptHandler, PIN_A7, flags);
	interrupt_AttachHandler(interruptHandler, PIN_A8, flags);
	interrupt_AttachHandler(interruptHandler, PIN_A9, flags);
	
	g_InterruptCount = 0;
	g_id = 0;
	g_mask = 0;
	
	while (true)
	{
		vPrintString("CNT = %lu, ID = %d, mask = 0x%08x\n", g_InterruptCount, g_id, g_mask);
		taskSleep(500);
	}
		
}
*/


///////////////////////////////////////////////////////////////////////////////
// void ApplicationTask(void *pvParameters)

void ApplicationTask(void *pvParameters)
{
	vPrintString("> ApplicationTask started\n");
	
	while (true)
	{
		//test_01_StatusInputTest();
		test_02_PingPongTestEncoders();
	}
	
	/* Should never go here */
	vTaskDelete(NULL);
}
