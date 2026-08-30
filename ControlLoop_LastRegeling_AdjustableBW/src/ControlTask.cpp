/*
 * ControlTask.c
 *
 * Created: 10-9-2023 09:48:15
 *  Author: rasmsmee
 */ 

///////////////////////////////////////////////////////////////////////////////
// system includes

#include <string.h>

///////////////////////////////////////////////////////////////////////////////
// application includes

#include "vprintf.h"

///////////////////////////////////////////////////////////////////////////////
// HAL includes for RTSW board

//#include "DeviceIOLib.h"
//#include "InterruptLib.h"
#include "bits.h"

///////////////////////////////////////////////////////////////////////////////
// position controller & application includes

//#include "PositionControllerLoad.h"
//#include "MotorControl.h"
//#include "QuadratureCounters.h"
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
	
	double wblFactor = 0.0;
	
	vPrint("> starting ControlTask (load)\n");

#if 0
	motor_DisableESCONController();

	// setup external 1 ms timer tick handler:
	
	TimerInterruptSemaphore = xSemaphoreCreateCounting(maxSemCount, initialSemCount);
	flags = PIO_IT_RISE_EDGE;
	interrupt_AttachHandler(ClockInterruptHandler, PIN_30, flags);
#endif
	vPrint("> ControlTask waiting for helper tasks...\n");
#if 0
	// wait for ButtonHandlerTask and ParameterSettingTask to get up and running:
	uxBits = xEventGroupWaitBits(handle_ThreadEventGroup, BIT_1 | BIT_0,
								 clearAllbits, waitForAllbits, ticksToWait);	

	vPrint("> helper tasks running, ControlTask started, event group = 0x%04x\n", uxBits);
	
	QCEncodersSetup();
	
	motor_EnableESCONController(); 
	motor_GotoHomePosition(MOVE_LEFT); 

	QCEncodersClearCount();
	QCEncodersShowCount("> Initial home");

	vPrint("> ready\n");
	vPrint("> press button SW1 to start (watch your fingers...)\n");
	
	ticksToWait = portMAX_DELAY;
	xSemaphoreTake(handle_RestartSemaphore, ticksToWait);	// wait for SW1 first button press
#endif

	while (true)
	{
#if 0
		motor_GotoHomePosition(MOVE_LEFT);

		QCEncodersClearCount();
		QCEncodersShowCount("> HOME");
		
		// always leave parameter value in queue! So use xQueuePeek
		ticksToWait = 0;
		xQueuePeek(handle_ParameterQueue, &wblFactor, ticksToWait);
		vPrint("> running with wblFactor: %.3f\n", wblFactor);
		
		posctrlLoad_InitParameters(wblFactor);
#endif
		ControlLoop();	// this loop exits by pressing button SW1
	}
	
	/* Should never go here */
	vTaskDelete(NULL);
}


///////////////////////////////////////////////////////////////////////////////
//  void ControlLoop(void)

void ControlLoop(void)
{
	vPrint("> enter Control Loop (load)\n");

	BaseType_t restart = pdFALSE;
	BaseType_t ticksToWait = 0;
	bool continueControlLoop = true;
	
	while (continueControlLoop)
	{
		#if 0
		// wait for periodic 1 ms timer tick to unblock this thread and
		// run the motion controller:
		xSemaphoreTake(TimerInterruptSemaphore, portMAX_DELAY);
		posctrlLoad_RunController();
		
		// check restart semaphore here, invoked by button press
		restart = xSemaphoreTake(handle_RestartSemaphore, ticksToWait);
		if (restart == pdTRUE)
		{
			continueControlLoop = false;
		}
		#endif
		vTaskDelay(0);
	}

	vPrint("> exit Control Loop (load)\n");
}
