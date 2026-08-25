/*
 * ControlTask.c
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
#include "InterruptLib.h"
#include "bits.h"

///////////////////////////////////////////////////////////////////////////////
// position controller & application includes

#include "PositionController.h"
#include "MotorControl.h"
#include "QuadratureCounters.h"
#include "ButtonHandlerTask.h"
#include "ControlTask.h"
#include "ApplicationTasks.h"


///////////////////////////////////////////////////////////////////////////////
// file globals

static SemaphoreHandle_t TimerInterruptSemaphore = NULL;


///////////////////////////////////////////////////////////////////////////////
// void ClockInterruptHandler(uint32_t id, uint32_t mask)
//
// invoked on every clock tick (1 ms) of the external hardware clock

void ClockInterruptHandler(uint32_t id, uint32_t mask)
{
	if (TimerInterruptSemaphore != NULL)
	{
		xSemaphoreGiveFromISR(TimerInterruptSemaphore, NULL);
	}
}


///////////////////////////////////////////////////////////////////////////////
// void ControlTask(void *pvParameters)

void ControlTask(void *pvParameters)
{
	uint32_t flags = 0;
	uint32_t maxSemCount = 1;
	uint32_t initialSemCount = 0;

	EventBits_t uxBits = 0;
	BaseType_t waitForAllbits = pdTRUE;
	BaseType_t clearAllbits	  = pdFALSE;
	TickType_t ticksToWait	  = portMAX_DELAY;
	
	double wbmFactor = 0.0;
	
	vPrintString("> starting ControlTask\n");

	motor_DisableESCONController();

	// setup external 1 ms timer tick handler:
	
	TimerInterruptSemaphore = xSemaphoreCreateCounting(maxSemCount, initialSemCount);
	flags = PIO_IT_RISE_EDGE;
	interrupt_AttachHandler(ClockInterruptHandler, PIN_30, flags);
	
	vPrintString("> ControlTask waiting for helper tasks...\n");

	// wait for ButtonHandlerTask and ParameterSettingTask to get up and running:
	uxBits = xEventGroupWaitBits(handle_ThreadEventGroup, BIT_1 | BIT_0,
								 clearAllbits, waitForAllbits, ticksToWait);	

	vPrintString("> helper tasks running, ControlTask started, event group = 0x%04x\n", uxBits);
	
	QCEncodersSetup();
	
	motor_EnableESCONController(); 
	motor_GotoHomePosition(MOVE_LEFT); 

	QCEncodersClearCount();
	QCEncodersShowCount("> Initial home");

	vPrintString("> ready\n");
	vPrintString("> press button SW1 to start (watch your fingers...)\n");
	
	ticksToWait = portMAX_DELAY;
	xSemaphoreTake(handle_RestartSemaphore, ticksToWait);	// wait for SW1 first button press
	
	while (true)
	{
		motor_GotoHomePosition(MOVE_LEFT);

		QCEncodersClearCount();
		QCEncodersShowCount("> HOME");
		
		// always leave parameter value in queue! So use xQueuePeek
		ticksToWait = 0;
		xQueuePeek(handle_ParameterQueue, &wbmFactor, ticksToWait);
		vPrintString("> running with wbmFactor: %.3f\n", wbmFactor);
		
		posctrl_InitParameters(wbmFactor);
		ControlLoop();	// this loop exits by pressing button SW1
	}
	
	/* Should never go here */
	vTaskDelete(NULL);
}


///////////////////////////////////////////////////////////////////////////////
//  void ControlLoop(void)

void ControlLoop(void)
{
	vPrintString("> enter Control Loop\n");

	BaseType_t restart = pdFALSE;
	BaseType_t ticksToWait = 0;
	bool continueControlLoop = true;
	
	while (continueControlLoop)
	{
		// wait for periodic 1 ms timer tick to unblock this thread and
		// run the motion controller:
		xSemaphoreTake(TimerInterruptSemaphore, portMAX_DELAY);
		posctrl_RunController_MotorSide();
		
		// check restart semaphore here, invoked by button press
		restart = xSemaphoreTake(handle_RestartSemaphore, ticksToWait);
		if (restart == pdTRUE)
		{
			continueControlLoop = false;
		}

		taskSleep(0);
	}

	vPrintString("> exit Control Loop\n");
}
